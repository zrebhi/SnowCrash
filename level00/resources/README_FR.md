# Niveau 00 - Trouver le mot de passe

## Approche initiale

En commençant le projet Snow Crash, nous devons trouver un moyen de passer de l'utilisateur `level00` à l'utilisateur `flag00`. Chaque niveau commence par la recherche du mot de passe pour l'utilisateur `flagXX` correspondant.

## Étape 1 : Recherche des fichiers appartenant à flag00

Puisque nous cherchons le mot de passe de l'utilisateur `flag00`, une étape logique est de rechercher les fichiers appartenant à l'utilisateur `flag00` :

```bash
find / -user flag00 2>/dev/null
```

Cette commande recherche dans tout le système de fichiers les fichiers appartenant à `flag00` et ignore les messages d'erreur.

## Étape 2 : Trouver des fichiers suspects

L'étape 1 a révélé un fichier qui contenait ce qui semblait être du texte encodé :

```
cdiiddwpgswtgt
```

## Étape 3 : Décodage avec le Chiffre de César

Comme le texte semblait encodé, j'ai essayé d'utiliser une approche de Chiffre de César pour le décoder. Un Chiffre de César est un simple chiffre de substitution où chaque lettre du texte clair est décalée d'un certain nombre de positions vers le bas ou vers le haut dans l'alphabet.

J'ai écrit une boucle pour essayer les 25 décalages possibles de l'alphabet :

```python
#!/usr/bin/env python3

# Cette fonction décale chaque lettre du texte d'un certain nombre de positions dans l'alphabet
def caesar_shift(text, shift_amount):
    result = ""
    
    for character in text:
        if character.isalpha():
            ascii_value = ord(character)
            alphabet_position = ascii_value - 97  # 97 est la valeur ASCII de 'a'            
            new_position = (alphabet_position + shift_amount) % 26            
            new_ascii_value = new_position + 97            
            new_character = chr(new_ascii_value)
            result += new_character
        else:
            # Si ce n'est pas une lettre, conserver le caractère tel quel
            result += character
    
    return result

encoded_text = "cdiiddwpgswtgt"

# Essayer tous les décalages possibles de 1 à 25
print("Trying all possible Caesar cipher shifts:")
for shift in range(1, 26):
    decoded_text = caesar_shift(encoded_text, shift)
    print(f"Shift {shift}: {decoded_text}")
```

L'une des sorties a clairement révélé le mot de passe :

```
Shift 11: nottoohardhere
Shift 12: opuuppibteifsf
Shift 13: pqvvqqjcufjgtg
...
```

Le décalage qui a produit une phrase anglaise lisible était la réponse.

## Étape 4 : Accéder au compte flag00

En utilisant le mot de passe décodé, j'ai pu passer à l'utilisateur `flag00` :

```bash
su flag00
# Password: nottoohardhere
```

Ensuite, j'ai exécuté la commande `getflag` :

```bash
getflag
```
Cela a révélé le jeton nécessaire pour passer au niveau 01 : x24ti5gi3x0ol2eh4esiuxias
