#include <time.h>
#include <stdio.h>
#include <stdlib.h>


/* ------------------------------ Type global ------------------------------- */

struct casejeu
{
    unsigned int numero ;
    struct casejeu * suivant ;
    struct casejeu * destination ;
} ;

typedef struct casejeu CaseJeu ;


/* -------------------------------------------------------------------------- */
/* --- Fonctions relatives à la liste chainée constituant la grille du jeu -- */

// Fonction de création d'une case
CaseJeu * CreerCase (unsigned int num)
{
    // Allocation mémoire pour une variable de type CaseJeu
    CaseJeu * Case = (CaseJeu *) malloc (sizeof (CaseJeu)) ;

    // Si allocation mémoire réussie, initialisation des paramètres
    if (Case != NULL)
    {
        Case->numero = num ;
        Case->suivant = NULL ;
        Case->destination = NULL ;
    }

    // Renvoi du pointeur sur la case créée
    return Case ;
}


// Fonction d'insertion d'une case dans la liste chaînée, appelée par la fonction
// InsertionTriee (fonctionne en début, au milieu et en fin de liste).
// ptr_case : adresse du pointeur sur la case avant laquelle insérer la nouvelle case
// Pas de vérification de l'existence d'une case de même numéro ici, celle-ci est
// faite dans InsertionTriee.
// Pour les besoins du programme, la fonction retourne le pointeur sur la case créée
CaseJeu * InsererCase (CaseJeu ** ptr_case, unsigned int num)
{
    // Création d'une case
    CaseJeu * nouvelle_case = CreerCase (num) ;

    // Insertion de la case créée dans la liste
    // (initialisation du champ 'suivant' de la case et mise à jour du pointeur dont l'adresse est fournie sur cette case)
    if (nouvelle_case != NULL)
    {
        nouvelle_case->suivant = *ptr_case ;
        *ptr_case = nouvelle_case ;
    }

    // Renvoi du pointeur sur la nouvelle case insérée
    return nouvelle_case ;
}


// Fonction d'insertion d'une case dans la liste chainée dans l'ordre des numéros.
// ptr_grille : adresse du pointeur sur la liste chaînée constituant la grille.
// Pour les besoins du programme (mise à jour à venir du champ 'destination' de la
// case insérée dans l'ordre), la fonction renvoie le pointeur sur cette case.
// La case peut déjà exister, dans ce cas la fonction renvoie l'adresse de cette case.
CaseJeu * InsertionTriee (CaseJeu ** ptr_grille, unsigned int num)
{
    CaseJeu * nouvelle_case ;
    CaseJeu * case_courante = *ptr_grille ;

    if ((case_courante == NULL) || (case_courante != NULL && num < case_courante->numero))
    // Si (la liste est vide) OU (elle n'est pas vide ET le numéro fourni est inférieur à celui de la case de tête de liste)
    {
        nouvelle_case = InsererCase (ptr_grille, num) ;  // on insère en tête la case avec le numéro fourni
    }
    else if (num == case_courante->numero)  // sinon, si la 1ère case contient le numero fourni,
    {
        nouvelle_case = case_courante ;    // on renvoie le pointeur sur cette case
    }
    else // sinon (la liste n'est pas vide, la case n'est pas à ajouter en tête et le numéro fourni ne correspond pas à la 1ère case),
    {
        CaseJeu * case_precedente = case_courante ;
        case_courante = case_courante->suivant ;
        while (case_courante != NULL && num > case_courante->numero)  // on parcourt la liste pour trouver la position où insérer (éventuellement) la case correspondant au numéro fourni
        {
            case_precedente = case_courante ;
            case_courante = case_courante->suivant ;
        }  // On sort de boucle parce qu'on a atteint la fin de liste ou parce que le numéro fourni est inférieur ou égal au numéro d'une case
        if ((case_courante == NULL) || (case_courante != NULL && num < case_courante->numero))
        {  // Si on a atteint la fin de liste ou si le numéro fourni est strictement inférieur au numéro d'une case
            nouvelle_case = InsererCase (&(case_precedente->suivant), num) ;  // on insère une case avec le numéro fourni
        }
        else  // sinon (=> le numéro fourni est égal au numéro d'une case (située après la case de tête de liste)),
        {
            nouvelle_case = case_courante ;  // on renvoie le pointeur sur cette case
        }
    }

    // Renvoi du pointeur sur la case insérée dans l'ordre ou de la case existante de même numéro
    return nouvelle_case ;
}


// Fonction de déplacement dans la grille : parcours de la liste depuis le début
// jusqu'au numéro fourni en argument. Renvoie le pointeur sur la case atteinte.
// Pas de vérification de dépassement de la dernière cellule, une vérification
// sur le numéro fourni est faite dans le moteur de jeu.
// Une méthode plus efficace consisterait à partir de la case courante et à se
// déplacer d'un certain nombre de cases (le résultat du lancé de dés). Mais cela
// ne fonctionnerait pas (avec la structure de données choisie) dans le cas où
// le lancé de dés est tel que l'on doit reculer par rapport à la position courante
// dans la grille. Il faudrait utiliser une liste doublement chainée pour cela.
CaseJeu * deplacementDansGrille (CaseJeu * debut, unsigned int numero)
{
    CaseJeu * Case = debut ;
    unsigned int num = 1 ;
    while (num < numero)
    {
        Case = Case->suivant ;
        num++ ;
    }
    return Case ;
}


// Fonction donnant le nombre de cases de la grille (utilisée dans le moteur de jeu,
// après la création de la grille, ce nombre n'étant pas une donnée d'entrée du jeu)
unsigned int nombreCasesGrille (CaseJeu * grille)
{
    CaseJeu * case_courante = grille ;
    unsigned int nb_cases = 0 ;
    while (case_courante != NULL)
    {
        nb_cases++ ;
        case_courante = case_courante->suivant ;
    }
    return nb_cases ;
}


// Fonction d'affichage de la liste, essentiellement utilisée pour vérifier le bon
// fonctionnement de la fonction lire_fichier_grille.
// Affiche une ligne par case avec pour chaque le numéro de la case et le numéro
// de la case destination vers laquelle elle renvoie éventuellement.
void AfficheListe (CaseJeu * grille)
{
    CaseJeu * case_courante = grille ;
    while (case_courante != NULL)
    {
        printf ("%u", case_courante->numero) ;
        if (case_courante->destination != NULL)
            printf (" --> %u", (case_courante->destination)->numero) ;
        printf ("\n") ;
        case_courante = case_courante->suivant ;
    }
}


// Fonction de suppression de la grille du jeu. Suppression globale de la grille
// ne passant pas par une fonction de suppression des différentes cases, la grille
// étant destinée à être utilisée avec le même nombre de cases pendant tout le
// déroulement du jeu et à être supprimée en une fois à la fin de celui-ci.
void supprimer_grille (CaseJeu * grille)
{
    CaseJeu * parcours = grille ;
    CaseJeu * case_courante ;
    while (parcours != NULL)
    {
        case_courante = parcours ;
        parcours = parcours->suivant ;
        free (case_courante) ;
    }
}



/* ---------------- Fonctions relatives au déroulement du jeu --------------- */

// randomize : initialise le générateur aléatoire
void randomize (void) {
    srand ((unsigned) time(NULL)) ;
}

// lancer_un_de : retourne un entier choisi aléatoirement entre 1 et 6
int lancer_un_de (void) {
    return (rand() % 6) + 1 ;
}



/* ------------------------- Fonctions à compléter  ------------------------- */

// lire_fichier_grille :
// Lit le fichier texte pour le nom donné en argument
// Crée la structure de données représentant la grille
// Retourne le pointeur sur cette structure de données
CaseJeu * lire_fichier_grille (char * nom_fichier)
{
    // Ouverture du fichier en lecture
    FILE * f = fopen (nom_fichier, "r") ;

    // Gestion de l'éventuelle erreur d'ouverture du fichier
    if (f == NULL)
    {
        fprintf (stderr, "Ouverture du fichier %s impossible - arrêt de l'exécution\n", nom_fichier) ;
        exit (EXIT_FAILURE) ;
    }

    // Parcours du fichier et création de la grille par insertion dans la liste chainée des cases rencontrées
    CaseJeu * grille = NULL ;
    unsigned int num1, num2 ;
    while (!feof(f))
    {
        fscanf (f, "%u %u\n", &num1, &num2) ;  // lecture d'une ligne
        CaseJeu * case1 = InsertionTriee (&grille, num1) ;  // insertion triée (éventuelle) de la case en 1ère position
        if (case1 == NULL)  // gestion de l'éventuelle erreur de création de la case
        {
            fprintf (stderr, "Création de la grille impossible : case manquante - arrêt de l'exécution\n") ;
            exit (EXIT_FAILURE) ;
        }
        if (num2 != 0)  // si une case doit renvoyer vers une autre,
        {
            CaseJeu * case2 = InsertionTriee (&grille, num2) ;  // insertion triée (éventuelle) de la case en 2è position
            if (case2 == NULL)  // gestion d'erreur de création de la case
            {
                fprintf (stderr, "Création de la grille impossible : case manquante - arrêt de l'exécution\n") ;
                exit (EXIT_FAILURE) ;
            }
            case1->destination = case2 ;  // mise à jour du champ 'destination' de la 1ère case de la ligne lue vers la 2è case de la ligne
        }
    }

    fclose (f) ;
    return grille ;
}


// jouer_jeu :
// Moteur de jeu s'éxécutant en fonction de la grille donnée en argument
// Le jeu évolue en fonction des entrées du joueur
void jouer_jeu (CaseJeu * grille)
{
    // Initialisations
    unsigned int nb_cases_grille = nombreCasesGrille (grille) ;
    char partie ;     // réponse de l'utilisateur pour le lancement d'une nouvelle partie
    char poursuite ;  // réponse de l'utilisateur pour la poursuite du jeu
    unsigned int numero_case, numero_case_dest ;
    unsigned int de1, de2, des ;
    CaseJeu * case_courante ;

    // ------------------------- Déroulement du jeu ------------------------- //
    printf ("Bienvenue dans le jeu Serpents et Echelles !\n") ;

    // Lancement des parties
    partie = '1' ;
    while (partie == '1')  // Nouvelle partie
    {
        // Initialisations
        numero_case = 1 ;
        case_courante = grille ;
        printf ("\nDébut de la partie !\n") ;
        printf ("Vous êtes en case %u\n", numero_case) ;

        // Lancement des premiers dés
        printf ("Pour lancer les dés, tapez 1 - pour quitter, tapez 0 : ") ;
        scanf (" %c", &poursuite) ;
        while (!(poursuite=='0' || poursuite=='1'))
        {
            printf ("Tapez 1 pour lancer les dés, 0 pour arrêter : ") ;
            scanf (" %c", &poursuite) ;
        }
        if (poursuite == '0')
        {
            partie = '0' ;  // on arrête la partie en cours ET on quitte le jeu
        }

        // Poursuite de la partie
        while (poursuite == '1')
        {
            // Lancé des deux dés
            de1 = lancer_un_de() ;
            de2 = lancer_un_de() ;
            des = de1 + de2 ;
            printf ("Les dés donnent %u + %u = %u\n", de1, de2, des) ;

            // Case atteinte
            numero_case = numero_case + des ;
            if (numero_case == nb_cases_grille)  // la case de destination est atteinte
            {
                case_courante = deplacementDansGrille (grille, numero_case) ; // pas absolument nécessaire
                printf ("Vous arrivez en case %u\n", numero_case) ;
                printf ("Bravo ! Vous êtes arrivé à destination, vous avez gagné !\n\n") ;

                // Lancement éventuel d'une nouvelle partie
                printf ("Voulez-vous recommencer une partie ? (Oui : tapez 1, non : tapez 0) : ") ;
                scanf (" %c", &partie) ;
                while (!(partie=='0' || partie=='1'))
                {
                    printf ("Tapez 1 pour commencer, 0 pour quitter le jeu : ") ;
                    scanf (" %c", &partie) ;
                }
                poursuite = '0' ;  // on sort du 1er while relatif à 'poursuite', on retombe dans le 2è relatif à 'partie'
            }
            else  // on n'a pas atteint la case de destination
            {
                if (numero_case > nb_cases_grille)  // on a dépassé la case de destination
                {
                    numero_case = nb_cases_grille - (numero_case - nb_cases_grille) ;
                    printf ("Lancés de dés trop élevé : vous reculez en case %u\n", numero_case) ;
                }
                else if (numero_case < nb_cases_grille)  // on est toujours avant la case de destination
                    printf ("Vous arrivez en case %u\n", numero_case) ;
                case_courante = deplacementDansGrille (grille, numero_case) ;

                // Eventuel changement de case si echelle ou serpent
                if (case_courante->destination != NULL)
                {
                    numero_case_dest = (case_courante->destination)->numero ;
                    if (numero_case_dest > numero_case)
                        printf ("Une échelle : vous avancez en case %u !\n", numero_case_dest) ;
                    else
                        printf ("Un serpent : vous reculez en case %u !\n", numero_case_dest) ;
                    case_courante = case_courante->destination ;
                    numero_case = case_courante->numero ;
                }

                // Poursuite du jeu en cours
                printf ("Pour lancer les dés, tapez 1 - pour quitter, tapez 0 : ") ;
                scanf (" %c", &poursuite) ;
                while (!(poursuite=='0' || poursuite=='1'))
                {
                    printf ("Tapez 1 pour lancer les dés, 0 pour arrêter : ") ;
                    scanf (" %c", &poursuite) ;
                }
                if (poursuite == '0')
                {
                    partie = '0' ;  // on arrête la partie en cours ET on quitte le jeu
                }
            }
        }
    }
    printf ("\nFin du jeu - à bientôt !\n") ;
}


/* --------------------------- Fonction principale -------------------------- */
int main (void)
{
    // Initialisation du jeu
    randomize() ;
    CaseJeu * grille = lire_fichier_grille ("serpents-echelles.txt") ;

    // Affichage de la liste
    // printf ("Nombre de cases de la grille : %u\n", nombreCasesGrille(grille)) ;
    // AfficheListe (grille) ;

    // Lancement du jeu
    jouer_jeu (grille) ;

    // Suppression de la grille
    supprimer_grille (grille) ;

    return EXIT_SUCCESS ;
}
