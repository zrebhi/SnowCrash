# Niveau 01 - Trouver le mot de passe

## Découverte initiale

En examinant le système à la recherche de vulnérabilités de sécurité potentielles, j'ai vérifié le fichier `/etc/passwd`, qui contient les informations des comptes utilisateurs :

```bash
level01@SnowCrash:~$ cat /etc/passwd
```

## La découverte

En examinant le contenu du fichier, j'ai trouvé quelque chose d'inhabituel dans l'entrée pour `flag01` :

```
flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
```

Contrairement à la plupart des autres utilisateurs qui affichent un `x` dans le champ du mot de passe (indiquant que le mot de passe est stocké dans `/etc/shadow`), l'utilisateur `flag01` a ce qui semble être un hash de mot de passe réel directement dans le fichier : `42hDRfypTqqnw`.

## Comprendre la vulnérabilité

C'est une vulnérabilité de sécurité significative car :

1. Le fichier `/etc/passwd` est lisible par tous les utilisateurs.
2. Les systèmes modernes stockent généralement les hashs de mots de passe dans `/etc/shadow` qui a des permissions restreintes.
3. Trouver un hash réel dans `/etc/passwd` signifie qu'il peut être extrait et potentiellement craqué.

## Craquer le mot de passe

Pour craquer ce hash de mot de passe, j'ai utilisé John the Ripper, un outil populaire de craquage de mots de passe :

1. D'abord, j'ai extrait la ligne pertinente de `/etc/passwd` :

   ```bash
   echo "flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash" > flag01.txt
   ```

2. Ensuite, j'ai utilisé John the Ripper pour craquer le mot de passe :

   ```bash
   john flag01.txt
   ```

3. Après un certain temps, John the Ripper a trouvé le mot de passe : `abcdefg`

## Accéder au compte flag01

Avec le mot de passe craqué, j'ai pu passer à l'utilisateur `flag01` :

```bash
su flag01
# Password: abcdefg
```

## Obtenir le drapeau

Finalement, j'ai exécuté la commande `getflag` pour récupérer le jeton pour le niveau suivant :

```bash
getflag
# Output: Check flag.Here is your token : f2av5il02puano7naaf6adaaf
```

## Conclusion

Ce niveau démontre l'importance des mécanismes de stockage appropriés des mots de passe. Stocker les hashs de mots de passe dans `/etc/passwd` est une vulnérabilité de sécurité significative car ce fichier est lisible par tous les utilisateurs du système. Les systèmes modernes utilisent `/etc/shadow` avec des permissions restreintes pour atténuer ce risque.
