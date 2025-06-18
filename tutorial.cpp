//
//  tutorial.cpp
//  RLTutorial
//
//  Created by Julio Godoy on 11/25/18.
//  Copyright © 2018 Julio Godoy. All rights reserved.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <string.h>

using namespace std;

int height_grid, width_grid, action_taken, action_taken2,current_episode;
int maxA[100][100], blocked[100][100];
float maxQ[100][100], cum_reward,Qvalues[100][100][4], reward[100][100],finalrw[50000];
int init_x_pos, init_y_pos, goalx, goaly, x_pos,y_pos, prev_x_pos, prev_y_pos, blockedx, blockedy,i,j,k;
ofstream reward_output;

//////////////
//Setting value for learning parameters
int action_sel=2; // 1 is greedy, 2 is e-greedy
int environment= 2; // 1 is small grid, 2 is Cliff walking
int algorithm = 2; //1 is Q-learning, 2 is Sarsa
int stochastic_actions=1; // 0 is deterministic actions, 1 for stochastic actions
int num_episodes=200; //total learning episodes
float learn_rate=0.1; // how much the agent weights each new sample
float disc_factor=0.99; // how much the agent weights future rewards
float exp_rate=0.05; // how much the agent explores (epsilon-greedy)
///////////////


void Initialize_environment()
{
    if(environment==1)
    {
        height_grid= 3;
        width_grid=4;
        goalx=3;
        goaly=2;
        init_x_pos=0;
        init_y_pos=0;

    }
    if(environment==2)
    {
    
        height_grid= 4;
        width_grid=12;
        goalx=11;
        goaly=0;
        init_x_pos=0;
        init_y_pos=0;

    }
    for(i=0; i < width_grid; i++)
    {
        for(j=0; j< height_grid; j++)
        {
            if(environment==1)
            {
                reward[i][j]=-0.04; //-1 if environment 2
                blocked[i][j]=0;
            }
            if(environment==2)
            {
                reward[i][j]=-1;
                blocked[i][j]=0;
            }
            for(k=0; k<4; k++)
            {
                Qvalues[i][j][k]=rand()%10;
                cout << "Initial Q value of cell [" <<i << ", " <<j << "] action " << k << " = " << Qvalues[i][j][k] << "\n";
            } 
        }  
    }
    if(environment==1)
    {
        reward[goalx][goaly]=100;
        reward[goalx][(goaly-1)]=-100;
        blocked[1][1]=1;
    }
    if(environment==2)
    {
        reward[goalx][goaly]=1;
        for(int h=1; h<goalx;h++)
        {   
            reward[h][0]=-100; 
        }
    }
}



int action_selection()
{ // Based on the action selection method chosen, it selects an action to execute next
    if(action_sel==1) //Greedy, always selects the action with the largest Q value
    {
        // Greedy: elegir acción con mayor Q-value
        int best_action = 0;
        float best_q = Qvalues[x_pos][y_pos][0];
        for(int a=1; a<4; a++) {
            if(Qvalues[x_pos][y_pos][a] > best_q) {
                best_q = Qvalues[x_pos][y_pos][a];
                best_action = a;
            }
        }
        return best_action;
    }

    if(action_sel==2)//epsilon-greedy, selects the action with the largest Q value with prob (1-exp_rate) and a random action with prob (exp_rate)
    {
        float r = ((float) rand()) / RAND_MAX;
        if(r < exp_rate) {
            // Exploración: acción aleatoria
            return rand()%4;
        } else {
            // Explotación: acción con mayor Q-value
            int best_action = 0;
            float best_q = Qvalues[x_pos][y_pos][0];
            for(int a=1; a<4; a++) {
                if(Qvalues[x_pos][y_pos][a] > best_q) {
                    best_q = Qvalues[x_pos][y_pos][a];
                    best_action = a;
                }
            }
            return best_action;
        }
    }
    return 0;
}

void move(int action)
{
    prev_x_pos=x_pos;
    prev_y_pos=y_pos;
    if(stochastic_actions)
    {
        float r = ((float) rand()) / RAND_MAX;
        if(r < 0.8) {
            // acción original, no cambia
        } else if(r < 0.9) {
            // 10%: derecha de la dirección deseada
            action = (action + 1) % 4;
        } else {
            // 10%: izquierda de la dirección deseada
            action = (action + 3) % 4; 
        }
    }
    if(action==0) // Up
    {
        if((y_pos<(height_grid-1))&&(blocked[x_pos][y_pos+1]==0)) //If there is no wall or obstacle Up from the agent
        {
            y_pos=y_pos+1;  //move up
        }
    }
    if(action==1)  //Right
    {
        if((x_pos<(width_grid-1))&&(blocked[x_pos+1][y_pos]==0)) //If there is no wall or obstacle Right from the agent
        {
            x_pos=x_pos+1; //Move right
        }
    }
    if(action==2)  //Down
    {
        if((y_pos>0)&&(blocked[x_pos][y_pos-1]==0)) //If there is no wall or obstacle Down from the agent
        {
            y_pos=y_pos-1; // Move Down
        }
    }
    if(action==3)  //Left
    {
        if((x_pos>0)&&(blocked[x_pos-1][y_pos]==0)) //If there is no wall or obstacle Left from the agent
        {
            x_pos=x_pos-1;//Move Left
        }
    }
}

void update_q_prev_state()
{
    if(!( ((x_pos==goalx)&&(y_pos==goaly)) ||((environment==1)&&(x_pos==goalx)&&(y_pos==(goaly-1)))||((environment==2)&&(x_pos>0)&&(x_pos<goalx)&&(y_pos==0))) )
    {
        float maxQnext = Qvalues[x_pos][y_pos][0];
        for(int a=1; a<4; a++) {
            if(Qvalues[x_pos][y_pos][a] > maxQnext)
                maxQnext = Qvalues[x_pos][y_pos][a];
        }
        Qvalues[prev_x_pos][prev_y_pos][action_taken] =
            Qvalues[prev_x_pos][prev_y_pos][action_taken] +
            learn_rate * (reward[x_pos][y_pos] + disc_factor * maxQnext - Qvalues[prev_x_pos][prev_y_pos][action_taken]);
    }
    else
    {
        Qvalues[prev_x_pos][prev_y_pos][action_taken] =
            Qvalues[prev_x_pos][prev_y_pos][action_taken] +
            learn_rate * (reward[x_pos][y_pos] - Qvalues[prev_x_pos][prev_y_pos][action_taken]);
    }
}

void update_q_prev_state_sarsa()
{
    if(!( ((x_pos==goalx)&&(y_pos==goaly)) ||((environment==1)&&(x_pos==goalx)&&(y_pos==(goaly-1)))||((environment==2)&&(x_pos>0)&&(x_pos<goalx)&&(y_pos==0))     ) )
    {
        Qvalues[prev_x_pos][prev_y_pos][action_taken] =
            Qvalues[prev_x_pos][prev_y_pos][action_taken] +
            learn_rate * (reward[x_pos][y_pos] + disc_factor * Qvalues[x_pos][y_pos][action_taken2] - Qvalues[prev_x_pos][prev_y_pos][action_taken]);
    }
    else
    {
        Qvalues[prev_x_pos][prev_y_pos][action_taken] =
            Qvalues[prev_x_pos][prev_y_pos][action_taken] +
            learn_rate * (reward[x_pos][y_pos] - Qvalues[prev_x_pos][prev_y_pos][action_taken]);
    }
}

void Qlearning()
{
    action_taken = action_selection();
    move(action_taken);
    update_q_prev_state();
    cum_reward = cum_reward + reward[x_pos][y_pos];
}

void Sarsa()
{
    move(action_taken);
    action_taken2 = action_selection();
    update_q_prev_state_sarsa();
    action_taken = action_taken2;
    cum_reward = cum_reward + reward[x_pos][y_pos];
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    // Permitir seleccionar ambiente y algoritmo por argumentos
    if(argc >= 2) environment = atoi(argv[1]);
    if(argc >= 3) algorithm = atoi(argv[2]);

    // Nombre de archivo de salida según ambiente y algoritmo
    string algo_str = (algorithm == 1) ? "qlearning" : "sarsa";
    string env_str = (environment == 1) ? "env1" : "env2";
    string filename = "reward_" + algo_str + "_" + env_str + ".csv";
    reward_output.open(filename);
    reward_output << "episode,reward\n";

    Initialize_environment();

    for(i=0;i<num_episodes;i++)
    {
        cout << "\n \n Episode " << i;
        current_episode=i;
        x_pos=init_x_pos;
        y_pos=init_y_pos;
        cum_reward=0;

        if(algorithm==2)
        {
            action_taken= action_selection();
        }

        while(!( ((x_pos==goalx)&&(y_pos==goaly)) ||((environment==1)&&(x_pos==goalx)&&(y_pos==(goaly-1)))||((environment==2)&&(x_pos>0)&&(x_pos<goalx)&&(y_pos==0))     ) )
        {
            if(algorithm==1)
            {
                Qlearning();
            }
            if(algorithm==2)
            {
                Sarsa();
            }
        }

        finalrw[i]=cum_reward;
        cout << " Total reward obtained: " <<finalrw[i] <<"\n";
        reward_output << i << "," << finalrw[i] << "\n";
    }
    reward_output.close();

    return 0;
}