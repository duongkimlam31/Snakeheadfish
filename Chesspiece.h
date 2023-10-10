#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#ifndef CHESSPIECE_H 
#define CHESSPIECE_H

class Chesspiece {
    protected:
        std::string position;
        int points;
        std::string status;
        std::string team;
        std::string icon;
        std::string name;
        std::vector<std::string> availableMoves;
    public:
        virtual void generateMoves(const std::unordered_map<std::string, std::string> occupiedCells){}
        virtual bool changeColor(){
            return false;
        }
        virtual ~Chesspiece(){};

        Chesspiece& operator=(const Chesspiece& other) {
        if (this == &other) {
            return *this; // Handle self-assignment
        }

            this->position = other.position;
            this->points = other.points;
            this->status = other.status;
            this->team = other.team;
            this->icon = other.icon;
            this->name = other.name;
            this->availableMoves = other.availableMoves;

            return *this; 
        }

        std::string getPosition(){
            return this->position;
        }
        std::string getStatus(){
            return this->status;
        }
        std::string getName(){
            return this->name;
        }
        std::string getTeam(){
            return this->team;
        }
        std::string getIcon(){
            return this->icon;
        }
        int getPoints(){
            return this->points;
        }
        std::vector<std::string> getAvailableMoves(){
            return this->availableMoves;
        }
        void setPosition(std::string position){
            this->position = position;
        }
        void setStatus(std::string status){
            this->status = status;
        }
        void setTeam(std::string team){
            this->team = team;
        }
        void setIcon(std::string icon){
            this->icon = icon;
        }
        void setPoints(int points){
            this->points = points;
        }
        void setName(std::string name){
            this->name = name;
        }
        void removeAllMoves(){
            this->availableMoves.clear();
        }
};


#endif