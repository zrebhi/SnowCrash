# Solution Niveau 09 - Inversion de la Transformation du Jeton

## Analyse

1. Nous avons un binaire `level09` qui s'exécute avec les permissions de `flag09`.
2. Il y a un fichier `token` dans le répertoire personnel avec un contenu binaire.
3. Lorsque nous exécutons `./level09 token`, cela affiche `tpmhr`.

## Processus d'Investigation

En comparant le contenu du fichier `token` avec la sortie du programme :

- Contenu du token (avec données binaires) : `b'f4kmm6p|=\x82\x7fp\x82n\x83\x82DB\x83Du{\x7f\x8c\x89'`
- Sortie du programme lorsqu'on lui donne `token` : `tpmhr`

Nous remarquons que le programme n'affiche pas tout le contenu du token. Cela suggère qu'il transforme l'entrée d'une manière ou d'une autre.

Tester avec des entrées simples a confirmé le motif de transformation :

```
./level09 aaaaa → abcde
./level09 11111 → 12345
```

Cela révèle que le programme ajoute l'index de position du caractère à sa valeur ASCII :

- 'a' (position 0) → 'a' (pas de changement)
- 'a' (position 1) → 'b' (incrémenté de 1)
- 'a' (position 2) → 'c' (incrémenté de 2)
  Et ainsi de suite...

## Solution

Pour obtenir le drapeau original, nous devons inverser cette transformation en soustrayant la position de chaque caractère de sa valeur ASCII :

```python
# Fonction pour décoder les données encodées
def decode(encoded_data):
    decoded_string = ""

    # Itérer sur chaque caractère et sa position
    for position, char_value in enumerate(encoded_data):
        # Soustraire la position de la valeur ASCII pour obtenir le caractère original
        decoded_character = chr(char_value - position)
        decoded_string += decoded_character

    return decoded_string
```

En appliquant cela au contenu du token, nous obtenons le jeton décodé complet : `f3iji1ju5yuevaus41q1afiuq`

## Étapes pour Résoudre

1. Créer et exécuter le script Python pour décoder le token : `python decoder.py`
2. Utiliser le mot de passe décodé (`f3iji1ju5yuevaus41q1afiuq`) pour se connecter en tant que `flag09` : `su flag09`
3. Exécuter `getflag` pour obtenir le jeton pour le niveau suivant : `s5cAJpM8ev6XHw998pRWG728z`
