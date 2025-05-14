# Niveau 14 : Rétro-ingénierie du Binaire getflag

Ce document explique l'approche pour le niveau 14, le dernier niveau du défi SnowCrash.

## 1. Évaluation Initiale

En accédant au niveau 14, nous n'avons trouvé aucun fichier spécial ou indice dans le répertoire home :

```bash
level14@SnowCrash:~$ ls -la
total 12
dr-x------ 1 level14 level14  100 Mar  5  2016 .
d--x--x--x 1 root    users    340 Aug 30  2015 ..
-r-x------ 1 level14 level14  220 Apr  3  2012 .bash_logout
-r-x------ 1 level14 level14 3518 Aug 30  2015 .bashrc
-r-x------ 1 level14 level14  675 Apr  3  2012 .profile
```

Sans vecteur d'attaque évident et étant le niveau final, nous avons conclu qu'il fallait examiner directement le binaire `getflag`.

## 2. Rétro-ingénierie du Binaire getflag

Nous avons utilisé Ghidra, un décompilateur, pour analyser le binaire `getflag` et comprendre son fonctionnement interne :

```bash
level14@SnowCrash:~$ which getflag
/bin/getflag
```

Après décompilation, nous avons découvert que :

1. Le binaire contenait des **mesures anti-débogage** utilisant `ptrace` pour empêcher l'analyse
2. Il effectuait diverses **vérifications anti-falsification** pour LD_PRELOAD et l'injection de bibliothèques
3. Il utilisait `getuid()` pour déterminer quel utilisateur exécutait le programme
4. En fonction de l'ID utilisateur, il déchiffrait et affichait le drapeau approprié
5. Tous les jetons pour tous les niveaux étaient **codés en dur sous forme de chaînes chiffrées** dans le binaire

## 3. Comprendre la Structure

La section clé du code décompilé montrait comment les drapeaux étaient déterminés :

```c
__uid_t user_id = getuid();

// Détermine quel drapeau afficher en fonction de l'ID utilisateur
switch (user_id) {
    case 0: // root
        fwrite("You are root are you that dumb ?\n", 1, 33, stdout); // Updated size
        break;
    // ... cas pour les autres niveaux ...
    case 3014: // level14/flag14 (Updated case)
        fputs(ft_des("g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|"), stdout);
        break;
    default:
        fwrite("\nNope there is no token here for you sorry. Try again :)",
               1, 56, stdout); // Updated size
        break;
}
```

Nous avons identifié que le drapeau pour level14 était encodé comme `"g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|"` et était déchiffré en utilisant une fonction appelée `ft_des()`.

## 4. Méthode d'Exploitation

Plutôt que d'essayer de contourner la vérification UID en utilisant GDB (ce qui était compliqué en raison des mesures anti-débogage), nous avons opté pour une approche plus simple :

1. **Extraire la fonction de déchiffrement** : Nous avons extrait la fonction `ft_des()` du code décompilé
2. **Extraire la chaîne de drapeau chiffrée** : Nous avons identifié le drapeau chiffré pour level14
3. **Créer un programme autonome** : Nous avons développé un petit programme C qui utilise la fonction extraite pour déchiffrer directement le drapeau level14

Voici le programme de déchiffrement autonome que nous avons créé :

```c

char *ft_des(char *encrypted_string)
{
  // Cette fonction est extraite du binaire getflag
  // Voir decrypt_flag.c pour l'implementation complète.
  // Elle n'est pas très importante, nous avons juste besoin de la réutiliser pour déchiffrer le drapeau
}


int main(void)
{
  // Le jeton chiffré flag14 extrait du binaire getflag
  char *encrypted = "g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|";
  
  // Déchiffrer le jeton directement
  char *decrypted = ft_des(encrypted);
  
  // Afficher le résultat
  printf("Jeton flag14 : %s\n", decrypted);
  
  // Libère la mémoire allouée
  free(decrypted);
  
  return 0;
}
```

## 5. Exécution et Résultats

Nous avons compilé et exécuté notre programme pour obtenir le drapeau :

```bash
level14@SnowCrash:~$ gcc -o decrypt_flag14 decrypt_flag14.c
level14@SnowCrash:~$ ./decrypt_flag14
Jeton flag14 : 7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ
```

Pour vérifier que c'était correct, nous avons utilisé le jeton pour nous connecter en tant que flag14 :

```bash
level14@SnowCrash:~$ su flag14
Password: 7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ
Congratulation. Type getflag to get the key and send it to me the owner of this livecd :)
flag14@SnowCrash:~$ getflag
Check flag.Here is your token : 7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ
```

## 6. Enseignements Clés

Ce niveau a démontré plusieurs concepts de sécurité importants :

1. **La sécurité par l'obscurité n'est pas efficace** : Même avec des mesures anti-débogage, le binaire pouvait être analysé et sa fonctionnalité extraite
2. **Les secrets intégrés sont vulnérables** : Coder en dur des secrets chiffrés directement dans les binaires est risqué car l'algorithme de déchiffrement peut être extrait
3. **Défense en profondeur** : Le programme employait plusieurs mécanismes de protection, mais contourner un seul (en extrayant la logique de déchiffrement) était suffisant pour vaincre l'ensemble du modèle de sécurité
4. **Approches alternatives** : Parfois, le vecteur d'attaque le plus simple (extraction et réutilisation de code) est plus efficace que des approches complexes (manipulation de l'exécution du programme avec des débogueurs)

En analysant la fonctionnalité du binaire et en extrayant juste ce dont nous avions besoin, nous avons pu obtenir le jeton final et compléter le défi SnowCrash !
