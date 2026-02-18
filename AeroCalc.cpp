//Importing necessary header files
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std;

// Used for error handling of invalid inputs
int getSafeChoice(int min, int max) {
    int choice;
    while (!(cin>> choice) || choice < min || choice > max) {
        cout << "Invalid! Enter " << min << " - " << max << " only: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    return choice;
}

// Making necessary objects for use
struct Aircraft
{
    string name;
    double empty_weight;
    double thrust;
    double wing_area;
    double drag_coeff;
    double cl_max;
    string type;

    double get_total_weight(double payload, double fuel_capacity){
        double total_weight;
        total_weight = empty_weight + payload + fuel_capacity;
        return total_weight;
    }

};
struct Airport
{
    string code;
    double runway_length;
    double altitude;
    double friction_mu;
};

// Storing objects as vectors
vector<Aircraft> fleet;
vector<Airport> airports;

// Adding aircraft object to vector database and global database
void add_aircraft(){
    // initializing aircraft properties
    string aircraft_name, typ;
    double empty_weight, thrust, wing_area, drag_coeff, cl_max;

    cout<<"Enter Aircaft Name: "; cin>>aircraft_name;
    cout<<"Enter Aircraft empty weight: "; cin>>empty_weight;
    cout<<"Enter Engine Thrust: "; cin>>thrust;
    cout<<"Enter wing area of the Aircraft: "; cin>>wing_area;
    cout<<"Enter drag coefficient (Cd) of the Aircraft: "; cin>>drag_coeff;
    
    cout<<"Aircraft types:"<<endl<<"1. General Aviation (Cl_max = 1.6)\n2. Fighter (Cl_max = 1.3)\n3. Airliner (Cl_max = 2.2)\n4. UAV (Cl_max = 1.4)\n5. Custom"<<endl<<"Enter Your choice (1-5): ";
    int achoice = getSafeChoice(1,5);

}

void view_fleet(){

}

void add_airport(){

}

void view_airports(){

}

int main(){
    cout<<"---------------------- Welcome to AeroCalc , Your Takeoff Clearance Assistant ----------------------"<<endl;
    cout<<"===================================================================================================="<<endl;

    bool main_menu = true;

    do {

        // Main Menu interface
        cout<<"1. Manage Aircrafts "<<endl;
        cout<<"2. Manage Airports "<<endl;
        cout<<"3. Run Simulation "<<endl;
        cout<<"4. Exit "<<endl;

        cout<<"Enter your Choice (1-4): ";
        int main_choice = getSafeChoice(1 , 4);


        // Switching Menu
        switch (main_choice){

            case 1:{
            cout<<"1. Add Aircraft"<<endl<<"2. View Fleet"<<endl;
			cout<<"Enter your choice (1-2): ";
            int aircraft_choice = getSafeChoice(1,2);
            switch (aircraft_choice)
            {
            case 1:{
                add_aircraft();
                break;
			}
            case 2:{
                view_fleet();
                break;
			}
            default:{
                cout<<"Invalid Choice!";
                break;}
            } break;
		}

            case 2:{
            cout<<"1. Add Airport"<<endl<<"2. View Airports"<<endl;
            cout<<"Your Choice (1-2): ";
            int airport_choice = getSafeChoice(1,2);
            switch (airport_choice){
                case 1:{
                add_airport(); break;}

                case 2:{
                view_airports(); break;}

                default:{
                cout<<"Invalid Choice!"<<endl; break;}
            }
        }

            case 3:{
            cout<<"Coming Soon"<<endl<<"=================================================================================================="<<endl; break;}

            case 4:{
            main_menu = false; break;}

            default:{
            cout<<"Invalid Choice. Please choose Correct option. "<<endl;
            break;}
        }

    } while (main_menu);

    cout<<"Safe Flights!";
    return 0;
}
