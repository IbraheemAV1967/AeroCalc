//Importing necessary header files
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <limits>

using namespace std;

// Constants for takeoff calculations
const double G = 9.81;              // gravitational acceleration (m/s^2)
const double RHO_SL = 1.225;        // sea-level air density (kg/m^3)
const double P_SL = 101325.0;       // sea-level standard pressure (Pa)
const double R_AIR = 287.058;       // specific gas constant for dry air (J/(kg*K))

// Standard atmosphere air density calculation (troposphere, altitude in m)
double get_density(double altitude_m) {
    return RHO_SL * pow(1.0 - 2.2558e-5 * altitude_m, 4.2561);
}

// Precise air density using actual temperature and altitude (ideal gas law)
// Uses standard atmosphere for pressure, actual temperature for density
double get_density(double altitude_m, double temp_celsius) {
    double P = P_SL * pow(1.0 - 2.2558e-5 * altitude_m, 5.2561);
    double T_K = temp_celsius + 273.15;
    return P / (R_AIR * T_K);
}

// Used for error handling of invalid inputs
int getSafeChoice(int min, int max) {
    int choice;
    while (!(cin>> choice) || choice < min || choice > max) {
        cout << "Invalid! Enter " << min << " - " << max << " only: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return choice;
}

double getSafeNumeric(){
    double numeric;
    while (!(cin>> numeric)) {
        cout << "Invalid! Enter Valid numeric value!";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return numeric; 
}

// Making necessary objects for use
struct Aircraft{
    string name;
    double empty_weight;
    double thrust;            // Static thrust T0 (N)
    double wing_area;
    double cd0;               // Zero-lift drag coefficient (CD0)
    double K;                 // Induced drag factor (CD = CD0 + K*CL^2)
    double cl_max;
    string type;
    double thrust_lapse_a;    // Thrust lapse coefficient 'a' in T = T0 - a*V^2 (N/(m/s)^2)
    string thrust_model;      // "Constant" or "Variable"

    double get_total_weight(double payload, double fuel_capacity){
        double total_weight;
        total_weight = empty_weight + payload + fuel_capacity;
        return total_weight;
    }

    // Get thrust at a given airspeed
    double get_thrust_at(double V) const {
        return thrust - thrust_lapse_a * V * V;
    }

    Aircraft(string n, double ew, double thr, double wa, double c0, double k, double clm, string typ,
             double a_coeff = 0.0, string t_model = "Constant"){
        name = n;
        empty_weight = ew;
        thrust = thr;
        wing_area = wa;
        cd0 = c0;
        K = k;
        cl_max = clm;
        type = typ;
        thrust_lapse_a = a_coeff;
        thrust_model = t_model;
    }
};

struct Airport{
    string code;
    double runway_length;
    double altitude;
    string material;
    double friction_mu;

    Airport(string cd, double rl, double alt, string mat, double mu){
        code = cd;
        runway_length = rl;
        altitude = alt;
        material = mat;
        friction_mu = mu;
    }
};

// Storing objects as vectors
vector<Aircraft> fleet;
vector<Airport> airports;

// Functions to save aircrafts and airports to database
void save_aircraft(Aircraft airplane){}
void save_airport(Airport runway){}

// Adding aircraft object to vector database and global database
void add_aircraft(){
    // initializing aircraft properties
    string aircraft_name, typ;
    double empty_weight, thrust, wing_area, cd0, K, cl_max;
    double thrust_lapse_a = 0.0;
    string thrust_model_str = "Constant";

    // Getting values and properties from user
    cout<<"Enter Aircaft Name: "; cin>>aircraft_name ;
    cout<<"Enter Aircraft mass (kg): "; empty_weight = getSafeNumeric();
    cout<<"Enter static thrust T0 (N): "; thrust = getSafeNumeric();
    cout<<"Enter wing area of the Aircraft (m^2): "; wing_area = getSafeNumeric();
    cout<<"Enter zero-lift drag coefficient (CD0): "; cd0 = getSafeNumeric();
    cout<<"Enter induced drag factor (K): "; K = getSafeNumeric();
    
    cout<<"Aircraft types:"<<endl<<"1. General Aviation (Cl_max = 1.6)\n2. Fighter (Cl_max = 1.3)\n3. Airliner (Cl_max = 2.2)\n4. UAV (Cl_max = 1.4)\n5. Custom"<<endl<<"Enter Your choice (1-5): ";
    int achoice = getSafeChoice(1,5);
    switch (achoice)
    {
    case 1:{typ = "GA"; cl_max = 1.6; break;}
    case 2:{typ = "Fighter"; cl_max = 1.3; break;}
    case 3:{typ = "Airliner"; cl_max = 2.2; break;}
    case 4:{typ = "UAV"; cl_max = 1.4; break;}
    case 5:{typ = "Custom";
        cout<<"Enter your custom max lift coeffecient: ";
            cl_max = getSafeNumeric();
            break;}
    
    default:{cout<<"ERROR!";
        break;}
    }

    // Thrust model selection (Ref: Marchman Section 7.1)
    cout<<"\nThrust model:"<<endl;
    cout<<"1. Constant Thrust (T = T0)"<<endl;
    cout<<"2. Variable Thrust (T = T0 - a*V^2)"<<endl;
    cout<<"Enter your choice (1-2): ";
    int tmodel = getSafeChoice(1,2);

    if(tmodel == 2){
        thrust_model_str = "Variable";
        cout<<"\nHow to specify the thrust lapse coefficient 'a'?"<<endl;
        cout<<"1. Enter 'a' directly (in N/(m/s)^2)"<<endl;
        cout<<"2. Enter thrust at a known speed (program calculates 'a')"<<endl;
        cout<<"Your choice (1-2): ";
        int amethod = getSafeChoice(1,2);

        if(amethod == 1){
            cout<<"Enter thrust lapse coefficient 'a' (N/(m/s)^2): ";
            thrust_lapse_a = getSafeNumeric();
        } else {
            cout<<"Enter the known airspeed (m/s): ";
            double V_known = getSafeNumeric();
            cout<<"Enter the thrust at that speed (N): ";
            double T_known = getSafeNumeric();
            if(V_known > 0){
                thrust_lapse_a = (thrust - T_known) / (V_known * V_known);
                cout<<"Calculated 'a' = "<<thrust_lapse_a<<" N/(m/s)^2"<<endl;
            } else {
                cout<<"Speed must be > 0. Defaulting to constant thrust."<<endl;
                thrust_model_str = "Constant";
            }
        }

        if(thrust_lapse_a < 0){
            cout<<"Warning: 'a' is negative (thrust increases with speed). Setting to constant thrust."<<endl;
            thrust_lapse_a = 0.0;
            thrust_model_str = "Constant";
        }
    }

    // Aircraft created
    Aircraft airplane (aircraft_name, empty_weight, thrust, wing_area, cd0, K, cl_max, typ,
                       thrust_lapse_a, thrust_model_str);
    fleet.push_back(airplane);

    // Saving aircraft to file database
    char save;
    bool loop =true;
    while (loop){
        cout<<"Save aircraft to database? (y/n): ";
        cin>>save;
        switch(save){
            case 'y':{save_aircraft(airplane); loop = false; break;}
            case 'n':{cout<<"Aircraft usable for this session only. Not stored in database."<<endl;loop = false; break;}
            default:{cout<<"Enter y or n!";}
        }}
}

// Function to view aircrafts saved in database
void view_fleet(){
    cout << "\n FLEET (" << fleet.size() << " aircrafts):\n";
    if(fleet.empty()) {
        cout << "No aircrafts yet!\n";
    } else {
        for(int i = 0; i < fleet.size(); i++) {
            cout << i+1 << ". " << fleet[i].name 
                 << " | CL_max= " << fleet[i].cl_max 
                 << " | CD0= " << fleet[i].cd0
                 << " | K= " << fleet[i].K
                 << " | Type: " << fleet[i].type
                 << " | Mass= " << fleet[i].empty_weight << " kg"
                 << " | Thrust: " << fleet[i].thrust_model;
            if(fleet[i].thrust_model == "Variable")
                cout << " (a=" << fleet[i].thrust_lapse_a << ")";
            cout << endl;
        }
    }
    cout << "Press Enter..."; cin.ignore(); cin.get();
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}


// Airport functions. add_airport for adding airport to database. view_airports to view saved airports.
void add_airport(){
    string runway_code, runway_surface;
    double runway_length, runway_altitude, runway_friction_coeffecient;
    cout<<"-----------------------------------"<<endl;
    cout<<"Enter runway code: "; cin>>runway_code;
    cout<<"Enter runway length (m): "; runway_length = getSafeNumeric();
    cout<<"Enter runway altitude (m): "; runway_altitude = getSafeNumeric();

    cout<<"Runway surface types (Ref: Table 6.1, Marchman):"<<endl;
    cout<<"1. Concrete/Asphalt, dry    (mu = 0.02)"<<endl;
    cout<<"2. Hard Turf                (mu = 0.04)"<<endl;
    cout<<"3. Normal Turf, Short Grass (mu = 0.05)"<<endl;
    cout<<"4. Normal Turf, Long Grass  (mu = 0.07)"<<endl;
    cout<<"5. Soft Ground              (mu = 0.10)"<<endl;
    cout<<"6. Custom"<<endl;

    cout<<"Your choice (1-6): "; int mat_choice = getSafeChoice(1,6);
    
    switch (mat_choice){
        case 1:{runway_surface = "Concrete/Asphalt_Dry"; runway_friction_coeffecient = 0.02; break;}
        case 2:{runway_surface = "Hard_Turf"; runway_friction_coeffecient = 0.04; break;}
        case 3:{runway_surface = "Short_Grass"; runway_friction_coeffecient = 0.05; break;}
        case 4:{runway_surface = "Long_Grass"; runway_friction_coeffecient = 0.07; break;}
        case 5:{runway_surface = "Soft_Ground"; runway_friction_coeffecient = 0.10; break;}
        case 6:{
            cout<<"Enter custom surface name: "; cin>>runway_surface;
            cout<<"Enter custom friction coefficient: "; runway_friction_coeffecient = getSafeNumeric();
            break;}
        default:{cout<<"Invalid Choice!"<<endl;}
    }
    cout<<"-----------------------------------------"<<endl;
    Airport runway(runway_code, runway_length, runway_altitude, runway_surface, runway_friction_coeffecient);

    airports.push_back(runway);

    save_airport(runway);

    cout<<"Airport saved to Database for future use!"<<endl;
    cout<<"-----------------------------------------"<<endl;
}

void view_airports(){
    cout << "\nAirports: (" << airports.size() << " airports):\n";
    if(airports.empty()) {
        cout << "No airports yet!\n";
    } else {
        for(int i = 0; i < airports.size(); i++) {
            cout << i+1 << ". " << airports[i].code 
                 << " | Length = " << airports[i].runway_length
                 << " | Altitude = " << airports[i].altitude 
                 << " | Material: " << airports[i].material << endl;
        }
    }
    cout << "Press Enter..."; cin.ignore(); cin.get();
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}

// Menu options are separate functions to make the main() function simpler

// Aircraft Management
void manage_aircraft(){
            cout<<"-------------------------"<<endl;
            cout<<"========================="<<endl;
            cout<<"Aircraft Management Menu"<<endl<<"=========================="<<endl;
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
            }
            cout<<"*************************"<<endl; 
}

// Airport Management
void manage_airport(){
            cout<<"--------------------------"<<endl;
            cout<<"=========================="<<endl;
            cout<<"Airport Management Menu"<<endl<<"=========================="<<endl;
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

// Simulation
void simulation(){
    // Check prerequisites
    if(fleet.empty()){
        cout<<"No aircraft in fleet! Please add an aircraft first."<<endl;
        return;
    }
    if(airports.empty()){
        cout<<"No airports available! Please add an airport first."<<endl;
        return;
    }

    bool retry_sim = true;
    while(retry_sim){
    retry_sim = false;

    cout<<"\n============ TAKEOFF SIMULATION ============"<<endl;

    // Select Aircraft
    cout<<"--- Select Aircraft ---"<<endl;
    for(int i = 0; i < fleet.size(); i++){
        cout << i+1 << ". " << fleet[i].name << " (" << fleet[i].type << ")" << endl;
    }
    cout<<"Your choice: ";
    int ac = getSafeChoice(1, fleet.size());
    Aircraft& plane = fleet[ac-1];

    // Select Airport
    cout<<"--- Select Airport ---"<<endl;
    for(int i = 0; i < airports.size(); i++){
        cout << i+1 << ". " << airports[i].code 
             << " (Runway: " << airports[i].runway_length << " m, Alt: " 
             << airports[i].altitude << " m)" << endl;
    }
    cout<<"Your choice: ";
    int ap = getSafeChoice(1, airports.size());
    Airport& rwy = airports[ap-1];

    // Get payload and fuel
    cout<<"Enter payload mass (kg): ";
    double payload = getSafeNumeric();
    cout<<"Enter fuel mass (kg): ";
    double fuel = getSafeNumeric();

    // Get ambient temperature for precise air density calculation
    cout<<"Enter ambient temperature at runway (Celsius): ";
    double temp_C = getSafeNumeric();

    // ===== CALCULATIONS =====
    double total_mass = plane.get_total_weight(payload, fuel); // total mass in kg
    double W = total_mass * G;  // weight as force in Newtons (W = m * g)
    double rho = get_density(rwy.altitude, temp_C);
    double mu = rwy.friction_mu;
    double T0 = plane.thrust;
    double S = plane.wing_area;
    double CLmax = plane.cl_max;
    double CD0 = plane.cd0;
    double K = plane.K;

    // Stall speed: Vstall = sqrt(2W / (rho * S * CLmax))
    double Vstall = sqrt(2.0 * W / (rho * S * CLmax));

    // Takeoff speed: VTO = 1.2 * Vstall
    double VTO = 1.2 * Vstall;

    // Optimal ground-run lift coefficient: CLg = mu / (2K)
    double CLg = mu / (2.0 * K);

    // Drag coefficient at ground-run CL: CD = CD0 + K * CLg^2
    double CDg = CD0 + K * CLg * CLg;

    // A and B terms (Ref: Marchman Section 7.1)
    // Thrust model: T = T0 - a*V^2  (a = 0 for constant thrust)
    double a_coeff = plane.thrust_lapse_a;
    // A = g * (T0/W - mu)
    double A = G * (T0 / W - mu);
    // B = (g/W) * [0.5 * rho * S * (CDg - mu * CLg) + a]
    double B = (G / W) * (0.5 * rho * S * (CDg - mu * CLg) + a_coeff);

    // Safety check: A must be positive (thrust must exceed friction)
    if(A <= 0){
        cout<<"\n*** ERROR: Thrust is insufficient to overcome ground friction! ***"<<endl;
        cout<<"T/W ratio: " << T0/W << " | Required minimum (mu): " << mu << endl;
        cout<<"Takeoff is IMPOSSIBLE with these parameters."<<endl;
        cout<<"Would you like to restart the simulation with different inputs? (y/n): ";
        char ch; cin >> ch;
        if(ch == 'y' || ch == 'Y') { retry_sim = true; continue; }
        else return;
    }

    // Safety check: aircraft must be able to reach VTO
    if(A - B * VTO * VTO <= 0){
        cout<<"\n*** ERROR: Aircraft cannot accelerate to takeoff speed! ***"<<endl;
        cout<<"Drag and friction exceed available thrust before reaching VTO."<<endl;
        if(a_coeff > 0){
            double T_at_VTO = plane.get_thrust_at(VTO);
            cout<<"Thrust at VTO = " << T_at_VTO << " N (" << (T_at_VTO < 0 ? "NEGATIVE - thrust model invalid at this speed!" : "positive") << ")" << endl;
        }
        cout<<"Would you like to restart the simulation with different inputs? (y/n): ";
        char ch; cin >> ch;
        if(ch == 'y' || ch == 'Y') { retry_sim = true; continue; }
        else return;
    }

    // Safety check for variable thrust: thrust at VTO must be positive
    if(a_coeff > 0){
        double T_at_VTO = plane.get_thrust_at(VTO);
        if(T_at_VTO <= 0){
            cout<<"\n*** WARNING: Thrust at VTO is "<<T_at_VTO<<" N (non-positive)! ***"<<endl;
            cout<<"The thrust model T = T0 - a*V^2 predicts zero/negative thrust at takeoff speed."<<endl;
            cout<<"Check your thrust lapse coefficient 'a' = "<<a_coeff<<" N/(m/s)^2"<<endl;
            cout<<"Would you like to restart the simulation with different inputs? (y/n): ";
            char ch; cin >> ch;
            if(ch == 'y' || ch == 'Y') { retry_sim = true; continue; }
            else return;
        }
    }

    // Get wind speed with validation loop
    // atanh(x) requires |x| < 1, so |Vw| must be less than sqrt(A/B)
    // sqrt(A/B) is the aircraft's maximum achievable airspeed on the ground
    double sqrtAB = sqrt(A * B);
    double sqrtBA = sqrt(B / A);
    double max_wind = 1.0 / sqrtBA;  // = sqrt(A/B)

    double Vw;
    while(true){
        cout<<"Enter wind speed (m/s) [positive = headwind, negative = tailwind, 0 = no wind]: ";
        Vw = getSafeNumeric();
        if(fabs(Vw) * sqrtBA < 1.0){
            break;  // valid wind speed
        }
        cout<<"\n*** Wind speed too extreme for valid calculation! ***"<<endl;
        cout<<"The wind speed must be less than " << max_wind << " m/s (the aircraft's maximum ground-run airspeed)."<<endl;
        cout<<"Please enter a valid wind speed.\n"<<endl;
    }

    double STO;     // takeoff ground run distance
    double t_to;    // time for takeoff ground run

    // General wind formula from textbook Section 7.5 (Marchman)
    // Handles headwind (Vw > 0), tailwind (Vw < 0), and no-wind (Vw = 0) cases
    // When Vw = 0, this reduces to the standard no-wind formulas automatically

    // Time: t = (1/sqrt(AB)) * [atanh(VTO*sqrt(B/A)) - atanh(Vw*sqrt(B/A))]
    t_to = (1.0 / sqrtAB) * (atanh(VTO * sqrtBA) - atanh(Vw * sqrtBA));
    // Distance: S = (1/2B) * ln((A - B*Vw^2) / (A - B*VTO^2)) - Vw*t
    STO = (1.0 / (2.0 * B)) * log((A - B * Vw * Vw) / (A - B * VTO * VTO)) - Vw * t_to;

    // ===== OUTPUT RESULTS =====
    cout<<"\n=========== TAKEOFF SIMULATION RESULTS ==========="<<endl;
    cout<<"Aircraft:  " << plane.name << " (" << plane.type << ")" << endl;
    cout<<"Airport:   " << rwy.code << " | Surface: " << rwy.material << endl;
    cout<<"----------------------------------------------------"<<endl;
    cout<<"  Total Mass:                " << total_mass << " kg" << endl;
    cout<<"  Total Weight (W):          " << W << " N" << endl;
    cout<<"  Static Thrust (T0):        " << T0 << " N" << endl;
    cout<<"  Thrust Model:              " << plane.thrust_model;
    if(a_coeff > 0){
        cout << " (a = " << a_coeff << " N/(m/s)^2)";
    }
    cout << endl;
    if(a_coeff > 0){
        double T_VTO = plane.get_thrust_at(VTO);
        cout<<"  Thrust at VTO:             " << T_VTO << " N  (" << (T_VTO/T0)*100.0 << "% of T0)" << endl;
    }
    cout<<"  Thrust-to-Weight (T0/W):   " << T0/W << endl;
    cout<<"  Ambient Temperature:       " << temp_C << " C" << endl;
    cout<<"  Air Density (rho):         " << rho << " kg/m^3" << endl;
    cout<<"  Rolling Friction (mu):     " << mu << endl;
    cout<<"----------------------------------------------------"<<endl;
    cout<<"  Stall Speed (Vstall):      " << Vstall << " m/s  (" << Vstall * 3.6 << " km/h)" << endl;
    cout<<"  Takeoff Speed (VTO):       " << VTO << " m/s  (" << VTO * 3.6 << " km/h)" << endl;
    cout<<"  Ground Run CL (CLg):       " << CLg << endl;
    cout<<"  Ground Run CD:             " << CDg << endl;
    if(Vw > 0)
        cout<<"  Wind:                      " << Vw << " m/s (headwind)" << endl;
    else if(Vw < 0)
        cout<<"  Wind:                      " << -Vw << " m/s (tailwind)" << endl;
    else
        cout<<"  Wind:                      None" << endl;
    cout<<"----------------------------------------------------"<<endl;
    cout<<"  A (accel constant):        " << A << " m/s^2" << endl;
    cout<<"  B (drag constant):         " << B << " m^-1" << endl;
    cout<<"----------------------------------------------------"<<endl;
    cout<<"  TAKEOFF GROUND RUN:        " << STO << " m" << endl;
    cout<<"  TAKEOFF TIME:              " << t_to << " sec" << endl;
    cout<<"  RUNWAY AVAILABLE:          " << rwy.runway_length << " m" << endl;
    cout<<"----------------------------------------------------"<<endl;

    if(STO <= rwy.runway_length){
        double margin = rwy.runway_length - STO;
        double margin_pct = (margin / rwy.runway_length) * 100.0;
        cout<<"  >> TAKEOFF POSSIBLE"<<endl;
        cout<<"  >> Safety Margin: " << margin << " m (" << margin_pct << "%)" << endl;
    } else {
        double deficit = STO - rwy.runway_length;
        cout<<"  >> TAKEOFF NOT POSSIBLE"<<endl;
        cout<<"  >> Runway Deficit: " << deficit << " m" << endl;
        cout<<"  >> Consider: reducing weight, increasing thrust, or using a longer runway." << endl;
    }
    cout<<"===================================================="<<endl;

    } // end while(retry_sim)
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
                manage_aircraft();
                break;}

            case 2:{
                manage_airport();
                break;}

            case 3:{
                simulation();
                break;}

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