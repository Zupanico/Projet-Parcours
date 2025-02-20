/*
Projet: Defi du parcours
Equipe: P6-B
Auteurs: Evan Frappier, Nicolas Garant, Félix Bordeleau et Kanvali Bakayoko
Description: Permet de guider le robot dans le labyrinthe
Date: 09/10/2023
*/

#include <LibRobus.h> // Essentielle pour utiliser RobUS

// Variables globales et defines
// ============================
int ambiant = 5;
int signal = 4;
int rouge = 49;
int vert = 48;
int position;
int orientation;
int parcours;
int mur;
int compteurDroite = 0;
int compteurGauche = 0;
long distance = 6650;    // correspond a 50 cm
long double_distance = distance * 2;
long tour = 1960;     // correspond a 90 degres
int32_t valEncodeurL;
int32_t valEncodeurR;
float kp = 0.00005;
float ki = 0.000003;
float erreur = 0;
float erreurKI = 0;
float compteur = 0.05;
double valPID = 0;
float millisDebut = 0;
// ============================

// Fonctions
// ============================

// Corriger les valeurs des encodeurs pour que le robot avance droit
float PID()
{
float millisFin = millis();

if(millisFin-millisDebut >= 50.0 ){}
    valEncodeurL = abs(ENCODER_Read(LEFT));
    valEncodeurR = abs(ENCODER_Read(RIGHT));

    erreur =((valEncodeurR - valEncodeurL));
    erreurKI += erreur;

    valPID = (1 / compteur) * (kp * erreur + ki * erreurKI);
    Serial.println(valPID, 10);
    return valPID;
    millisDebut = millis();
}

// Avancer le robot d'une distance de 50 cm
void avancer()
{
    MOTOR_SetSpeed(RIGHT, 0.15);
    MOTOR_SetSpeed(LEFT,0.15);
    delay(100);

    while ((ENCODER_Read(LEFT) < distance) && (ENCODER_Read(RIGHT) < distance))
    {
        if ((ENCODER_Read(LEFT) < (distance * 0.10)) && (ENCODER_Read(RIGHT) < (distance * 0.10)))
        {
            MOTOR_SetSpeed(RIGHT, 0.15);
            MOTOR_SetSpeed(LEFT, 0.15 + PID());
        }
        else if ((ENCODER_Read(LEFT) > (distance * 0.10)) && (ENCODER_Read(RIGHT) > (distance * 0.10)) && (ENCODER_Read(LEFT) < (distance * 0.80)) && (ENCODER_Read(RIGHT) < (distance * 0.80)))
        {
            MOTOR_SetSpeed(RIGHT, 0.30);
            MOTOR_SetSpeed(LEFT, 0.30 + PID());
        }
        else
        {
            MOTOR_SetSpeed(RIGHT, 0.15);
            MOTOR_SetSpeed(LEFT, 0.15 + PID());
        }
    }
    arreter();

}

void avancer_double()
{   

    MOTOR_SetSpeed(RIGHT, 0.15);
    MOTOR_SetSpeed(LEFT,0.15);
    delay(100);

    while ((ENCODER_Read(LEFT) < (double_distance)) && (ENCODER_Read(RIGHT) < double_distance))
    {
        if ((ENCODER_Read(LEFT) < (double_distance * 0.10)) && (ENCODER_Read(RIGHT) < (double_distance * 0.10)))
        {
            MOTOR_SetSpeed(RIGHT, 0.15);
            MOTOR_SetSpeed(LEFT, 0.15 + PID());
        }
        else if ((ENCODER_Read(LEFT) > (double_distance * 0.10)) && (ENCODER_Read(RIGHT) > (double_distance * 0.10))&&(ENCODER_Read(LEFT) < (double_distance * 0.80)) && (ENCODER_Read(RIGHT) < (double_distance * 0.80)))
        {
            MOTOR_SetSpeed(RIGHT, 0.30);
            MOTOR_SetSpeed(LEFT, 0.30 + PID());
        }
        else
        {
            MOTOR_SetSpeed(RIGHT, 0.15);
            MOTOR_SetSpeed(LEFT, 0.15 + PID());
        }
    }
    arreter();
}

void arreter()
{
    MOTOR_SetSpeed(RIGHT, 0);
    MOTOR_SetSpeed(LEFT, 0);
    delay(100);
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
}
void tourner_droite()
{

    MOTOR_SetSpeed(RIGHT, -0.22);
    MOTOR_SetSpeed(LEFT, 0.22 + PID());

    while (ENCODER_Read(LEFT) < (tour - 53))
    {
        true;
    }


    arreter();
}
void tourner_gauche()
{

    MOTOR_SetSpeed(RIGHT, 0.22);
    MOTOR_SetSpeed(LEFT, -(0.22 + PID()));

    while (ENCODER_Read(RIGHT) < (tour - 25))
    {
        true;
    }


    arreter();
}
void demi_tour()
{

    MOTOR_SetSpeed(RIGHT, 0.22);
    MOTOR_SetSpeed(LEFT, -(0.22 + PID()));

    while (ENCODER_Read(RIGHT) < (2 * tour + 5))
    {
        true;
    }


    arreter();
}
void reculer_droite()
{
    avancer();
    parcours--;
    avancer();
    parcours--;
    tourner_gauche();
    if (digitalRead(rouge) == 0 || digitalRead(vert) == 0)
    {
        tourner_droite();
        avancer();
        parcours--;
        avancer();
        parcours--;
        tourner_gauche();
    }
}
void reculer_gauche()
{
    avancer();
    parcours--;
    avancer();
    parcours--;
    tourner_droite();
    if (digitalRead(rouge) == 0 || digitalRead(vert) == 0)
    {
        tourner_gauche();
        avancer();
        parcours--;
        avancer();
        parcours--;
        tourner_droite();
    }
}

void detectionDesifflet()
{
  
  double bruitAmbiant = analogRead(ambiant);
  double signal_5Khz = analogRead(signal);
  
  for (int i = 0; i < 500; i++)
  {
    while ((signal_5Khz - bruitAmbiant) > -200)
    {
       bruitAmbiant = analogRead(ambiant);
        signal_5Khz = analogRead(signal);
       Serial.println(signal_5Khz-bruitAmbiant);
       delay(1);
    }
  }
  delay(1500);
}

// Fonctions d'initialisation (setup)

void setup()
{
  Serial.begin(9600);
  BoardInit();
  pinMode(signal,  INPUT);
  pinMode(ambiant,  INPUT);
  pinMode(vert, INPUT);
  pinMode(rouge, INPUT);

  position = 2;    // le robot commence dans la colonne du milieu
  orientation = 0; // le robot commence en faisant face vers l'avant
  parcours = 1;    // le robot commence sur la premiere ligne
  mur = 0;

detectionDesifflet();

}

// Fonctions de boucle infini (loop())

void loop()
{
    // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
    delay(100); // Delais pour dÃ©charger le CPU

    while (parcours < 10)
    {
        if (((digitalRead(rouge) == 0 || digitalRead(vert) == 0)) || ((position == 2 && (parcours == 1 || parcours == 3 || parcours == 7 || parcours == 9))))
        {
            switch (position)
            {
            case 1: // le robot voit un mur et il est dans la colonne de gauche
                // va dans la colonne du milieu
                tourner_droite();
                avancer();
                position++;
                tourner_gauche();
                mur = 1;
                break;

            case 2:
                //il est dans la colonne du milieu et il voit un mur
                if(mur == 3)
                {
                    tourner_gauche();
                    if (digitalRead(rouge) == 0 || digitalRead(vert) == 0)
                    {
                        tourner_gauche();
                        reculer_gauche();
                        avancer();
                        position--;
                        tourner_droite();
                    }
                    else
                    {
                        avancer();
                        position--;
                        tourner_droite();
                    }   
                }
                else
                {                  
                    tourner_droite();

                    // verifie s'il y a un mur en avant et s'il y a un mur dans la colonne 1
                    if (digitalRead(rouge) == 0 || digitalRead(vert) == 0)
                    {
                        
                        if (mur == 1)
                        {
                            // verifie s'il y a un mur en avant et s'il y a un mur dans les autres colonnes
                            tourner_droite();
                            reculer_droite();
                            avancer();
                            position++;
                            tourner_gauche();
                        }
                        else
                        {
                            // Fait demi tour et se redresse
                            demi_tour();
                            avancer();
                            position--;
                            tourner_droite();
                        }
                        
                    }

                    else
                    {
                        avancer();
                        position++;
                        tourner_gauche();

                        if (digitalRead(rouge) == 0 || digitalRead(vert) == 0)
                        {
                            tourner_gauche();
                            avancer();
                            position--;

                            if (digitalRead(rouge) == 0 || digitalRead(vert) == 0)
                            {
                                tourner_gauche();
                                reculer_gauche();
                                avancer();
                                position--;
                                tourner_droite();
                            }
                            else
                            {
                                avancer();
                                position--;
                                tourner_droite();
                            }
                        }
                    }
                }
                // reset le mur
                mur = 0;
                break;

            case 3: // le robot voit un mur et il est dans la colonne de droite
                // va dans la colonne du milieu
                tourner_gauche();
                avancer();
                position--;
                tourner_droite();
                mur = 3;
                break;

            default:
                break;
            }
        }  
        else
        {
         
            if ((parcours %2 == 1) && (parcours!=9))
            {
                avancer_double();
                parcours += 2;
            }
            else
            {
                avancer();
                parcours++;
                mur = 0;
            }
        
        }
    }
       
    // le robot est rendu a la derniere ligne, il doit se rendre a la ligne d'arrivee et la depasser
    if (position==3) // s'il est dans la 3e colonne
    {
        avancer();
        tourner_gauche();
        avancer();
        tourner_gauche();

        for (int i = 0; i < 10; i++)
        {
            avancer();
        }
    }
    else if (position == 1) // s'il est dans la 1ere colonne
    {
        avancer();
        tourner_droite();
        avancer();
        tourner_droite();

        for (int i = 0; i < 10; i++)
        {
            avancer();
        }
    }

    // arrete le robot
    arreter();
}
// Fin du programme
// ============================