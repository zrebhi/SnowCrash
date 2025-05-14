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

Alternativement, nous pouvons utiliser GDB (GNU Debugger) pour manipuler le flux d'exécution et la mémoire du programme à l'exécution.

**Étapes :**

1.  **Démarrer GDB avec le binaire :**
    L'option `-q` démarre GDB en mode silencieux.
    ```bash
    level13@SnowCrash:~$ gdb -q ./level13
    Reading symbols from /home/user/level13/level13...(no debugging symbols found)...done.
    ```

2.  **Désassembler la fonction `main` :**
    Cela nous aide à comprendre la logique du programme et à trouver la vérification de l'UID.
    ```gdb
    (gdb) disas main
    Dump of assembler code for function main:
       0x0804858c <+0>:     push   %ebp
       0x0804858d <+1>:     mov    %esp,%ebp
       0x0804858f <+3>:     and    $0xfffffff0,%esp
       0x08048592 <+6>:     sub    $0x10,%esp
       0x08048595 <+9>:     call   0x8048380 <getuid@plt>
       0x0804859a <+14>:    cmp    $0x1092,%eax
       0x0804859f <+19>:    je     0x080485cb <main+63>
       0x080485a1 <+21>:    call   0x8048380 <getuid@plt>
       0x080485a6 <+26>:    mov    $0x80486c8,%edx
       0x080485ab <+31>:    movl   $0x1092,0x8(%esp)
       0x080485b3 <+39>:    mov    %eax,0x4(%esp)
       0x080485b7 <+43>:    mov    %edx,(%esp)
       0x080485ba <+46>:    call   0x8048360 <printf@plt>
       0x080485bf <+51>:    movl   $0x1,(%esp)
       0x080485c6 <+58>:    call   0x80483a0 <exit@plt>
       0x080485cb <+63>:    movl   $0x80486ef,(%esp)
       0x080485d2 <+70>:    call   0x8048474 <ft_des>
       0x080485d7 <+75>:    mov    $0x8048709,%edx
       0x080485dc <+80>:    mov    %eax,0x4(%esp)
       0x080485e0 <+84>:    mov    %edx,(%esp)
       0x080485e3 <+87>:    call   0x8048360 <printf@plt>
       0x080485e8 <+92>:    leave
       0x080485e9 <+93>:    ret
    End of assembler dump.
    ```
    D'après le désassemblage, nous voyons l'appel à `getuid()` à `0x08048595` et la comparaison à `0x0804859a` :
    ```assembly
       0x08048595 <+9>:     call   0x8048380 <getuid@plt>
       0x0804859a <+14>:    cmp    $0x1092,%eax
    ```
    Le résultat de `getuid()` est stocké dans le registre `%eax`, qui est ensuite comparé à `0x1092` (décimal 4242).

3.  **Placer un point d'arrêt juste après l'appel à `getuid()` et avant la comparaison :**
    Nous choisissons l'adresse `0x0804859a`, qui est l'instruction `cmp`. Cela nous permet de modifier `%eax` avant qu'il ne soit utilisé dans la comparaison.
    ```gdb
    (gdb) break *0x0804859a
    Breakpoint 1 at 0x0804859a
    ```

4.  **Exécuter le programme :**
    ```gdb
    (gdb) run main
    Starting program: /home/user/level13/level13 main

    Breakpoint 1, 0x0804859a in main ()
    ```
    Le programme s'arrête à notre point d'arrêt.

5.  **Modifier la valeur dans le registre `%eax` :**
    Dans l'architecture x86, les valeurs de retour des fonctions sont généralement stockées dans le registre `%eax`. Nous mettons `%eax` à l'UID attendu, `0x1092` (4242).
    ```gdb
    (gdb) set $eax=0x1092
    ```

6.  **Continuer l'exécution :**
    ```gdb
    (gdb) continue
    Continuing.
    your token is 2A31L79asukciNyi8uppkEuSx
    ```
    Avec la valeur UID modifiée dans `%eax`, la comparaison `cmp $0x1092,%eax` réussira, et le programme continuera pour déchiffrer et afficher le jeton.

Cette méthode ne nécessite pas de modifier le binaire lui-même, mais change plutôt le comportement du programme à l'exécution en manipulant un registre CPU.

## 5. Solution et Jeton

L'exécution du binaire avec la vérification de l'UID contournée a produit le jeton :

```
your token is 2A31L79asukciNyi8uppkEuSx
```

Ce jeton peut être utilisé pour se connecter en tant qu'utilisateur level14.
