# Niveau 02 - Analyse du trafic réseau

## Comprendre le défi

Au niveau 02, nous trouvons un fichier `.pcap` dans le répertoire personnel. Cela offre l'opportunité d'analyser le trafic réseau pour trouver des identifiants.

```bash
level02@SnowCrash:~$ ls -l
----r--r-- 1 flag02  level02 8302 Aug 30  2015 level02.pcap
```

## Qu'est-ce qu'un fichier PCAP ?

Un fichier PCAP (Packet Capture) contient des données de trafic réseau capturées par des outils comme tcpdump ou Wireshark. Ces fichiers enregistrent des conversations réseau complètes, incluant :

- Les adresses IP source et destination
- Les informations de protocole
- Le contenu des paquets (pouvant potentiellement contenir des données sensibles)
- Les horodatages de chaque paquet

## Installation de Wireshark

Pour analyser correctement le fichier PCAP, nous avons d'abord dû le transférer sur notre machine locale et utiliser un outil spécialisé :

```bash
# Transférer le fichier sur notre machine locale
scp -P 4242 level02@[VM_IP]:/home/user/level02/level02.pcap .

# Installer Wireshark sur la machine Linux locale
sudo apt install wireshark
```

## Suivre le flux TCP

Après avoir ouvert le fichier PCAP dans Wireshark, nous avons suivi ces étapes pour analyser les données :

1. Clic droit sur un paquet
2. Sélectionné "Follow > TCP Stream" (Suivre > Flux TCP)
3. Observé une session de connexion où quelqu'un tentait de se connecter

## Extraire le mot de passe

En regardant le flux TCP, nous pouvions voir qu'après une invite "Password:", des caractères étaient tapés un par un. En examinant attentivement chaque paquet, nous avons observé :

```
07:23:34.363418: 'f'
07:23:35.253053: 't'
07:23:35.873401: '_'
07:23:36.343811: 'w'
07:23:36.573585: 'a'
07:23:36.803330: 'n'
07:23:36.943261: 'd'
07:23:37.283708: 'r'
07:23:38.864101: '.' (retour arrière)
07:23:39.233935: '.' (retour arrière)
07:23:39.604364: '.' (retour arrière)
07:23:40.374542: 'N'
07:23:40.574439: 'D'
07:23:42.264451: 'R'
07:23:43.574954: 'e'
07:23:44.014684: 'L'
07:23:44.635281: '0'
07:23:44.805020: 'L'
```

## Comprendre les retours arrière

Les trois points consécutifs ('.') dans la capture représentent des caractères de retour arrière (backspace). L'utilisateur a tapé "ft_wandr", puis a appuyé trois fois sur retour arrière pour supprimer "ndr", et a ensuite tapé "NDReL0L".

Par conséquent, le mot de passe correct est : `ft_waNDReL0L`

## Obtenir le drapeau

Avec ce mot de passe, nous pouvions accéder au compte flag02 :

```bash
su flag02
# Password: ft_waNDReL0L
getflag
Output: Check flag.Here is your token : kooda2puivaav1idi4f57q8iq
```

## Conclusion

Ce niveau démontre l'importance des canaux de communication sécurisés. Envoyer des mots de passe sur des connexions non chiffrées permet à quiconque ayant accès au trafic réseau de capturer les identifiants. Utilisez toujours des protocoles chiffrés comme SSH, HTTPS ou des VPN pour les informations sensibles.
