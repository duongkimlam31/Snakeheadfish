#include <string>
#include <vector>
#include <unordered_map>

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
        virtual void generateMoves(const std::unordered_map<std::string, std::string> occupiedCells) = 0;
        virtual bool changeColor() = 0;
        virtual ~Chesspiece(){};
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