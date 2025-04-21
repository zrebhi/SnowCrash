# Niveau 13 : Contournement de la Vérification d'UID dans un Binaire

Ce document explique la vulnérabilité dans le binaire `level13` et les étapes suivies pour l'exploiter.

## 1. Évaluation Initiale

En accédant au niveau 13, nous avons trouvé un fichier exécutable binaire :

```bash
level13@SnowCrash:~$ ls -l
total 8
-rwsr-sr-x 1 flag13 level13 7303 Aug 30  2015 level13
```

L'exécution du binaire a montré qu'il recherchait un identifiant utilisateur spécifique :

```bash
level13@SnowCrash:~$ ./level13
UID 2013 started us but we we expect 4242
```

Pour comprendre comment ce binaire fonctionne, nous l'avons décompilé à l'aide de Ghidra (un puissant outil de rétro-ingénierie) afin d'obtenir sa structure de code source. Cela nous a permis de voir la logique interne et d'identifier les moyens de contourner sa protection.

## 2. Explication du Programme

D'après le code décompilé, nous avons appris que la cible est un programme en C qui déchiffre un jeton, mais uniquement s'il est exécuté par un utilisateur spécifique.

Éléments clés du programme :

- Il utilise `getuid()` pour vérifier l'ID de l'utilisateur actuel :

  ```c
  current_uid = getuid();
  if (current_uid != 4242) {
      printf("UID %d started us but we we expect %d\n", current_uid, 0x1092);
      exit(1);
  }
  ```

- Si la vérification de l'UID réussit, il appelle `ft_des()` pour déchiffrer le jeton :

  ```c
  decrypted_token = ft_des("boe]!ai0FB@.:|L6l@A?>qJ}I");
  printf("your token is %s\n", decrypted_token);
  ```

- La fonction `ft_des()` contient un algorithme de déchiffrement personnalisé qui traite l'entrée chiffrée caractère par caractère en utilisant des manipulations basées sur la position.

## 3. Analyse de la Vulnérabilité

La restriction principale est la **vérification de l'ID utilisateur** :

- Le programme ne déchiffre et n'affiche le jeton que s'il est exécuté par un utilisateur avec l'UID 4242
- Notre utilisateur level13 a probablement un UID différent

La fonction de déchiffrement `ft_des()` est autonome dans le binaire, donc si nous pouvons contourner la vérification de l'UID, nous pouvons obtenir le jeton.

## 4. Méthode d'Exploitation

Il existe plusieurs façons de contourner la vérification de l'UID :

### Méthode 1 : Décompilation et Modification du Binaire (Utilisée dans la Solution)

Après avoir décompilé le binaire avec Ghidra, nous avons pu :

**Étapes :**

1. **Extraire le code C décompilé complet** :

   - Avec le code décompilé de Ghidra, nous avons créé un fichier C (`level13.c`) contenant la logique complète du programme
   - Cela incluait à la fois la fonction principale avec la vérification de l'UID et la fonction personnalisée `ft_des()`

2. **Créer une version modifiée** en commentant la vérification de l'UID :

   ```c
   // current_uid = getuid();
   // if (current_uid != 4242) {
   //     printf("UID %d started us but we we expect %d\n", current_uid, 0x1092);
   //     exit(1);
   // }
   ```

3. **Compiler le code modifié** :

   ```bash
   gcc -o modified_level13 level13.c
   ```

4. **Exécuter le binaire modifié** :
   ```bash
   ./modified_level13
   your token is 2A31L79asukciNyi8uppkEuSx
   ```

### Méthode 2 : Utilisation de GDB pour Contourner la Vérification

Alternativement, nous avons utilisé GDB (GNU Debugger) pour manipuler le programme pendant l'exécution :

```bash
level13@SnowCrash:~$ gdb -q ./level13
```

Le flag `-q` démarre GDB en mode silencieux, supprimant les messages d'introduction et de copyright pour une sortie plus claire.

**Étapes :**

1. **Définir un point d'arrêt au début de la fonction principale :**

   ```
   (gdb) break main
   Breakpoint 1 at 0x804858f
   ```

2. **Démarrer le programme :**

   ```
   (gdb) run
   ```

3. **Trouver où l'appel getuid() se produit :**
   Lorsque nous arrivons au point d'arrêt, nous devons identifier où la fonction `getuid()` est appelée. Généralement, nous utiliserions `disas main` pour visualiser l'assemblage et identifier l'instruction.

4. **Modifier la valeur de retour de getuid() :**
   Dans l'architecture x86, les valeurs de retour des fonctions sont stockées dans le registre `eax`. Après l'exécution de `getuid()` mais avant la comparaison, nous définissons :

   ```
   (gdb) set $eax = 4242
   ```

   Cela change la valeur de retour de `getuid()` de notre UID réel (2013) à la valeur attendue (4242).

5. **Continuer l'exécution :**
   ```
   (gdb) continue
   ```
   Avec la valeur UID modifiée en place, le programme continue au-delà de la vérification et déchiffre le jeton :
   ```
   your token is 2A31L79asukciNyi8uppkEuSx
   ```

Cette méthode ne nécessite pas de modification du binaire lui-même, mais change plutôt le comportement du programme à l'exécution en manipulant un registre CPU.

## 5. Solution et Jeton

L'exécution du binaire avec la vérification de l'UID contournée a produit le jeton :

```
your token is 2A31L79asukciNyi8uppkEuSx
```

Ce jeton peut être utilisé pour se connecter en tant qu'utilisateur level14.
