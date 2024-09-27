#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <unordered_set>
#include <cctype>

using namespace std;

// structure to represent disease.
//stores the value from the vector diseases made in main function.
struct Disease
{
    string name;
    vector<string> symptoms;
    vector<string> treatments;
};

// Patient structure
struct Patient
{
    string patientId;
    string password;
    string firstName;
    string lastName;
    string dob;
    int age;
    char gender;
    string registrationDate;
    string mobileNumber;
};

// Function prototypes
string toLowercase(const string& str); // function to remove CASE SENSTITIVITY

//functions to do exception handling for different fields that are used to store the information of patients.
bool validateMobile(const string& mobile);
bool validateName(const string& name);
bool isValidDate(const string& date);
bool isNumeric(const string& str);

//in case Patient not found in the text file, so registration shall be done and hence ID shall be generated.
string generatePatientId(const vector<Patient>& patients);
void registerPatient(vector<Patient>& patients);

void writePatients(const vector<Patient>& patients);
vector<Patient> readPatients();

bool login(vector<Patient>& patients);

// Converts a string to lowercase
string toLowercase(const string& str)
{
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// converts a string to uppercase
string toUppercase(const string& str)
{
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Checks for the fields where only numeric values allowed.
bool isNumeric(const string& str)
{
    return !str.empty() && all_of(str.begin(), str.end(), ::isdigit);
}

// this function checks that the mobile number has exactly 10 digits
bool validateMobile(const string& mobile)
{
    return mobile.length() == 10 && isNumeric(mobile);
}

// this function doesn't allow any numeric value or special character in names
bool validateName(const string& name)
{
    return !name.empty() && all_of(name.begin(), name.end(), [](char c) { return isalpha(c) || c == ' '; });
}

// Checks if the date entered is in right format.
bool isValidDate(const string& date)
{
    if (date.length() != 10)
        return false;

    if (!isNumeric(date.substr(0, 2)) || !isNumeric(date.substr(3, 2)) || !isNumeric(date.substr(6, 4)))
        return false;

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    // checking that the date entered for DOB or registration is not the date of the future.
    if (month < 1 || month > 12 || day < 1 || day > 31)
        return false;

    //will return an error if the year greater than 2024
    if (year > 2024)
        return false;

    return true;
}

// Generates unique patient ID, with the pattern of PID0000 XX
string generatePatientId(const vector<Patient>& patients)
{
    if (patients.empty())
    {
        //the values of repeated 0 do not make a diff, the ID will work if PID01 is also entered.
        return "PID000001";
    }

    int maxId = 0;
    for (const Patient& p : patients)
    {
        int id = stoi(p.patientId.substr(3));
        if (id > maxId) {
            maxId = id;
        }
    }
    //addition of numeric value (the suffix) by 1 for unique ID for each patient.
    return "PID" + to_string(maxId + 1);
}

// Writes patients data to file when a new patient is registered.
void writePatients(const vector<Patient>& patients)
{
    ofstream file("patients.txt");
    if (file.is_open())
    {
        for (const Patient& p : patients)
        {
            file << p.patientId << "\t" << p.password << "\t" << p.firstName << "\t" << p.lastName << "\t" << p.dob << "\t" << p.age << "\t" << p.gender << "\t" << p.registrationDate << "\t" << p.mobileNumber <<endl;
        }
        file.close();
    }
}

// Reads patient data from file and adds it into a vector
vector<Patient> readPatients()
{
    vector<Patient> patients;
    //text file reading
    ifstream file("patients.txt");
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            istringstream iss(line);
            //data of patients is stored in a structure of the same name.
            Patient p;
            iss >> p.patientId >> p.password >> p.firstName >> p.lastName >> p.dob >> p.age >> p.gender >> p.registrationDate >> p.mobileNumber;
            patients.push_back(p);
        }
        file.close();
    }
    return patients;
}

bool login(vector<Patient>& patients, string& user_ID)
{
    cout<<"*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<<endl;
    cout<<"* WELCOME TO DISEASE IDENTIFYING SYSTEM *" <<endl;
    cout<<"*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<<endl;

    string userId, password;
    cout<<"Enter your user ID or mobile number: ";
    getline(cin, userId);

    auto it = find_if(patients.begin(), patients.end(), [&](const Patient& p) { return p.patientId == userId || p.mobileNumber == userId; });
    if (it != patients.end())
    {
        int attempts = 3; // Number of attempts allowed
        while (attempts > 0)
        {
            cout<<"Enter your password: ";
            getline(cin, password);
            if (password == it->password)
            {
                cout<<"\n*********************************************************************\n";
                cout<<"* LOGIN SUCCESSFUL!! WELCOME, " << it->firstName << " " << it->lastName << " *" <<endl;
                cout<<"***********************************************************************\n\n";
                user_ID = it->patientId; // Update user_ID with the logged-in user's ID
                return true;
            }
            else
            {
                attempts--;
                if (attempts > 0)
                {
                    cout<<"***************************************************"<<endl;
                    cout<<"* INCORRECT PASSWORD. " << attempts << " ATTEMPTS LEFT *" <<endl;
                    cout<<"***************************************************"<<endl;
                }
                else
                {
                    cout<<"************************************************"<<endl;
                    cout<<"*  INCORRECT PASSWORD. NO MORE ATTEMPTS LEFT.  *"<<endl;
                    cout<<"************************************************"<<endl;
                    break;
                }
            }
        }
    }
    else
    {
        char choice;
        cout<<"*****************"<<endl;
        cout<<"* INVALID CREDENTIALS. NO PATIENT FOUND. **"<<endl;
        cout<<"*****************"<<endl;
        cout<<"\nDo you want to register? (Y/N): ";
        cin >> choice;
        if (toupper(choice) == 'Y')
        {
            cin.ignore(); // Ignore newline character from previous input
            registerPatient(patients); // Call registerPatient function
            return true;
        }
    }

    return false;
}

// Register new patient function
void registerPatient(vector<Patient>& patients)
{
    Patient p;
    cout<<"*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<<endl;
    cout<<"\n** PATIENT REGISTRATION WINDOW **" <<endl;
    cout<<"*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<<endl;

    cout<<"Enter your first name: ";
    cin >> p.firstName;
    while (!validateName(p.firstName))
    {
        cout<<"ERROR!. Please re-enter your first name: ";
        cin >> p.firstName;
    }

    cout<<"Enter your last name: ";
    cin >> p.lastName;
    while (!validateName(p.lastName))
    {
        cout<<"ERROR!. Please re-enter your last name: ";
        cin >> p.lastName;
    }

    cout<<"Enter your age: ";
    cin >> p.age;
    while (p.age <= 0)
    {
        cout<<"ERROR!. Please re-enter your age: ";
        cin >> p.age;
    }

    cout<<"Enter your gender (M/F): ";
    cin >> p.gender;
    p.gender = toupper(p.gender);
    while (p.gender != 'M' && p.gender != 'F')
    {
        cout<<"ERROR!. Please re-enter your gender (M/F): ";
        cin >> p.gender;
        p.gender = toupper(p.gender);
    }

    cout<<"Enter your date of birth (DD/MM/YYYY): ";
    cin >> p.dob;
    while (!isValidDate(p.dob))
    {
        cout<<"ERROR!. Please re-enter your date of birth (DD/MM/YYYY): ";
        cin >> p.dob;
    }

    cout<<"Enter your mobile number: ";
    cin >> p.mobileNumber;
    while (!validateMobile(p.mobileNumber))
    {
        cout<<"ERROR!. Please re-enter your mobile number: ";
        cin >> p.mobileNumber;
    }

    cout<<"Enter a password: ";
    cin >> p.password;

    string confirmPwd;
    cout<<"Confirm your password: ";
    cin >> confirmPwd;
    while (confirmPwd != p.password)
    {
        cout<<"*******"<<endl;
        cout<<"* PASSWORDS DO NOT MATCH . RE - ENTER! **"<<endl;
        cout<<"*******"<<endl;

        cout<<"\nRe-enter your password: ";
        cin >> p.password;
        cout<<"Confirm your password: ";
        cin >> confirmPwd;
    }

    p.patientId = generatePatientId(patients);
    time_t now = time(0);
    char buf[100];
    strftime(buf, sizeof(buf), "%Y/%m/%d", localtime(&now));
    p.registrationDate = buf;

    patients.push_back(p);
    writePatients(patients);

    cout<<"*******************************************************************************"<<endl;
    cout<<"\n** R E G I S T R A T I O N   S U C C E S S F U L ! !   W E L C O M E, " << p.firstName << " **\n" <<endl;
    cout<<"*******************************************************************************"<<endl;
}

// Function to check if a symptom is present in a given list (case-insensitive)
bool hasSymptom(const vector<string>& symptomsList, const string& symptom)
{
    string lowerSymptom = toLowercase(symptom);
    for (const auto& s : symptomsList)
    {
        if (toLowercase(s) == lowerSymptom)
        {
            return true;
        }
    }
    return false;
}

// Function to ask yes/no question and return true for 'yes' answers (case-insensitive)
bool askYesNoQuestion(const string& question)
{
    string answer;
    while (true)
    {
        cout<<question << " (yes/no): ";
        getline(cin, answer);
        // Convert answer to lowercase before comparison
        transform(answer.begin(), answer.end(), answer.begin(), ::tolower);
        if (answer == "yes" || answer == "y")
        {
            return true;
        }
        else if (answer == "no" || answer == "n")
        {
            return false;
        }
        else
        {
            cout<<"Invalid input. Please enter 'yes' or 'no'." <<endl;
        }
    }
}

// Function to prompt user for symptoms selection when no common symptoms match
vector<string> selectSymptoms(const vector<string>& uncommonSymptoms)
{
    vector<string> selectedSymptoms;
    string userInput;

    cout<<"Please select the symptoms you are experiencing:" <<endl;

    // Display symptoms in a table format with three columns
    const int numColumns = 3;
    for (size_t i = 0; i < uncommonSymptoms.size(); ++i)
    {
        cout<<setw(2) << i + 1 << ". " << setw(20) << left << uncommonSymptoms[i];
        if ((i + 1) % numColumns == 0 || i == uncommonSymptoms.size() - 1)
        {
            cout<<endl;
        }
        else
        {
            cout<<"\t"; // Add tab character between columns
        }
    }

    while (true)
    {
        cout<<"Enter the numbers corresponding to the selected symptoms (separated by spaces): ";
        getline(cin, userInput);

        istringstream iss(userInput);
        int number;
        bool validInput = true;

        while (iss >> number)
        {
            if (number > 0 && number <= static_cast<int>(uncommonSymptoms.size()))
            {
                selectedSymptoms.push_back(uncommonSymptoms[number - 1]);
            }
            else
            {
                cout<<"Invalid selection. Please enter valid numbers from the menu." <<endl;
                validInput = false;
                selectedSymptoms.clear(); // Clear any previously selected symptoms
                break;
            }
        }

        // Check if input was valid (no letters or special characters)
        if (validInput)
        {
            // If all numbers were valid, break out of the loop
            break;
        }
    }

    return selectedSymptoms;
}

// Function to prompt user for symptoms and filter diseases based on symptoms
vector<Disease> identifyDiseases(const vector<Disease>& diseases)
{
    vector<Disease> matchingDiseases;

    // Common symptoms
    vector<string> commonSymptoms = {"fever", "body ache", "sore throat", "cold", "cough", "stomach ache", "fatigue"};
    vector<string> userSymptoms;

    // Ask about common symptoms
    cout<<"************************************"<<endl;
    cout<<"** DISEASE IDENTIFICATION WINDOW ***"<<endl;
    cout<<"************************************"<<endl;
    cout<<"\nLet's check for common symptoms:" <<endl;
    for (const auto& symptom : commonSymptoms)
    {
        if (askYesNoQuestion("Do you have " + symptom + "?"))
        {
            userSymptoms.push_back(symptom);
        }
    }

    // If less than two common symptoms selected, ask for specific symptoms
    if (userSymptoms.size() <= 1)
    {
        cout<<"\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - "<<endl;
        cout<<"Less than two common symptoms selected. Please select from uncommon symptoms." <<endl;
        // List uncommon symptoms
        vector<string> uncommonSymptoms;
        for (const auto& disease : diseases)
        {
            for (const auto& symptom : disease.symptoms)
            {
                if (!hasSymptom(commonSymptoms, symptom) && find(uncommonSymptoms.begin(), uncommonSymptoms.end(), symptom) == uncommonSymptoms.end())
                {
                    uncommonSymptoms.push_back(symptom);
                }
            }
        }

        // Check if there are any uncommon symptoms available
        if (uncommonSymptoms.empty())
        {
            cout<<"\nNo uncommon symptoms available for selection."<<endl;
            cout<<"******** E X I T I N G ********"<<endl;
            return matchingDiseases; // Return an empty list of diseases
        }

        // Let user select symptoms from uncommon list
        vector<string> selectedUncommonSymptoms = selectSymptoms(uncommonSymptoms);
        // Combine common and uncommon symptoms
        userSymptoms.insert(userSymptoms.end(), selectedUncommonSymptoms.begin(), selectedUncommonSymptoms.end());
    }

    // Filter diseases based on user symptoms
    for (const auto& disease : diseases)
    {
        int matchedSymptomCount = 0;
        for (const auto& userSymptom : userSymptoms)
        {
            if (hasSymptom(disease.symptoms, userSymptom))
            {
                matchedSymptomCount++;
            }
        }
        // Add disease to matchingDiseases if it matches at least one symptom
        if (matchedSymptomCount > 0)
        {
            matchingDiseases.push_back(disease);
        }
    }
    return matchingDiseases;
}

// Function to display detailed information about a disease
void viewDiseaseDetails(const Disease& disease, unordered_set<string>& viewedDiseases)
{
    cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" <<endl;
    cout<<"D I S E A S E: " << disease.name <<endl;
    cout<<"\nS Y M P T O M S: ";
    for (const auto& symptom : disease.symptoms)
    {
        cout<<symptom << ", ";
    }
    cout<<endl;
    cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" <<endl;

    // Add disease name to viewed set
    viewedDiseases.insert(disease.name);
}

// Function to display personal information of the logged-in user
void displayPersonalInformation(const string& loggedInPatientId, const vector<Patient>& patients)
{
    // Find the patient with the logged-in ID
    auto it = find_if(patients.begin(), patients.end(), [&](const Patient& p) { return p.patientId == loggedInPatientId; });

    if (it != patients.end())
    {
        const Patient& patient = *it;

        // Display personal information
        cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;
        cout<<"\n     P E R S O N A L    I N F O R M A T I O N :    "<<endl;
        cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;
        cout<<"Patient ID: " << patient.patientId <<endl;
        cout<<"First Name: " << patient.firstName <<endl;
        cout<<"Last Name: " << patient.lastName <<endl;
        cout<<"Date of Birth: " << patient.dob <<endl;
        cout<<"Age: " << patient.age <<endl;
        cout<<"Gender: " << patient.gender <<endl;
        cout<<"Mobile Number: " << patient.mobileNumber <<endl;
        cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;

    }
    else
    {
        cout<<"*************"<<endl;
        cout<<"--------* PATIENT NOT FOUND! ---------"<<endl;
        cout<<"*************"<<endl;
    }
}

// Function to suggest tests based on symptoms
void suggestTests(const vector<string>& symptoms)
{
    cout<<"\nBased on your symptoms, the following tests are suggested:" <<endl;
    cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;

    // Check if symptoms indicate potential heart disease
    if (find(symptoms.begin(), symptoms.end(), "chest pain") != symptoms.end() &&
        find(symptoms.begin(), symptoms.end(), "shortness of breath") != symptoms.end())
    {
        cout<<"- Electrocardiogram (ECG)" <<endl;
        cout<<"- Echocardiogram (Echo)" <<endl;
    }

    // Check if symptoms suggest possible diabetes mellitus
    if (find(symptoms.begin(), symptoms.end(), "increased thirst") != symptoms.end() &&
        find(symptoms.begin(), symptoms.end(), "frequent urination") != symptoms.end())
    {
        cout<<"- Fasting Plasma Glucose Test" <<endl;
        cout<<"- Oral Glucose Tolerance Test (OGTT)" <<endl;
    }

    // Check if symptoms are indicative of respiratory infections
    if (find(symptoms.begin(), symptoms.end(), "fever") != symptoms.end() &&
        find(symptoms.begin(), symptoms.end(), "cough") != symptoms.end() &&
        find(symptoms.begin(), symptoms.end(), "difficulty breathing") != symptoms.end())
    {
        cout<<"- Chest X-ray" <<endl;
        cout<<"- Pulmonary Function Tests (PFTs)" <<endl;
    }

    // If no specific patterns matched, suggest general tests
    if (symptoms.size() >= 3)
    {
        cout<<"- Complete Blood Count (CBC)" <<endl;
        cout<<"- Urine Analysis" <<endl;
    }
    cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;
}
// Bill Calculation
double calculateBill(int numPredicted, int numDetailsDisplayed, int numMedicationsDisplayed, int numYesResponses)
{
    //Price per service availed:
    const double predictionFee = 100.0; // Fee per disease prediction
    const double detailsFee = 50.0;     // Fee per disease details viewed
    const double medicationFee = 30.0;  // Fee per medication details viewed
    const double yesResponseFee = 45.0; // Fee per "yes" response

    //total bill calculation-
    double totalBill = predictionFee * numPredicted + detailsFee * numDetailsDisplayed + medicationFee * numMedicationsDisplayed + yesResponseFee * numYesResponses;

    return totalBill;
}


// Function to provide details of the doctor to consult
void provideDoctorDetails()
{
    cout<<"\nFor further diagnosis and consultation, it is recommended to see a general practitioner or an internist." <<endl;
    cout<<"You can visit your nearest health-care center or consult a doctor online." <<endl;
}

// Function to calculate change for cash payment
bool calculateChange(double billAmount, double cashAmount)
{
    // Ensure cash amount is greater than or equal to bill amount
    if (cashAmount < billAmount)
    {
        std::cout<<"Error: Insufficient cash provided." << std::endl;
        return false;
    }

    // Calculate change
    double change = cashAmount - billAmount;

    // Output change
    std::cout<<"Change: Rs. " << std::fixed << std::setprecision(2) << change << std::endl;
    return true;
}

// Function to simulate an Online payment transaction with bank details
bool processOnlinePayment(double billAmount)
{
    // Allowed bank names
    vector<string> bnk_names = {"ICICI Bank", "SBI", "Bank of Baroda", "Axis Bank", "HDFC Bank", "Kotak Mahindra Bank", "IDFC Bank"};

    // Bank details
    cout<<"Enter your bank details for Online payment:" <<endl;

    string bankName;
    cout<<"Bank Name: ";
    getline(cin, bankName);

    string uppercaseBankName = toUppercase(bankName);

    // Check if the entered bank name matches any allowed bank name or its substring
    bool validBank = false;
    for (const string& bnk : bnk_names) {
        string uppercaseBnk = toUppercase(bnk);
        if (uppercaseBankName.find(uppercaseBnk) != string::npos || uppercaseBnk.find(uppercaseBankName) != string::npos) {
            validBank = true;
            bankName = bnk; // Set bank name to the full name from the list
            break;
        }
    }

    if (!validBank) {
        cout<<"Invalid bank name. Please enter a valid bank name from the list." <<endl;
        return false;
    }

    string accountNumber;
    cout<<"Account Number: ";
    getline(cin, accountNumber);

    string cvv;
    cout<<"CVV: ";
    getline(cin, cvv);

    // Payment processing simulation-
    cout<<"Processing Online Payment through " << bankName << " bank..." <<endl;
    cout<<"\nPlease wait while we connect you to the " << bankName << " payment gateway." <<endl;
    cout<<"\nPayment authorization in progress..." <<endl;

    bool paymentSuccess = true;

    if (paymentSuccess)
    {
        cout<<"Payment of Rs. " << fixed << setprecision(2) << billAmount << " through " << bankName << " bank is successful." <<endl;
    }
    else
    {
        cout<<"Payment through " << bankName << " bank failed. Please try again later or choose a different payment method." <<endl;
        cout<<"Attempting payment again..." <<endl;

        // Retry payment
        cout<<"Processing Online Payment through " << bankName << " bank..." <<endl;
        cout<<"\nPlease wait while we connect you to the " << bankName << " payment gateway." <<endl;
        cout<<"\nPayment authorization in progress..." <<endl;

        // Second attempt
        paymentSuccess = (rand() % 2 == 0); // Randomly choose success or failure
        if (paymentSuccess)
        {
            cout<<"Second attempt successful! Payment of Rs. " << fixed << setprecision(2) << billAmount << " through " << bankName << " bank is successful." <<endl;
            return true;
        }
        else
        {
            cout<<"Second attempt failed. Payment through " << bankName << " bank failed again. Please try again later or choose a different payment method." <<endl;
            return false;
        }
    }
}

// to store the feedback given by the user.
struct Feedback
{
    int overallExperienceRating;
    string improvementSuggestions;
    bool concernsAddressed;
    bool enoughInformationProvided;
    bool treatmentEffective;
    bool sideEffectsExperienced;
    string additionalComments;
};

// Function to provide Feedback
void provideFeedback(Feedback &feedback)
{
    cout<<"****************\n";
    cout<<"*      F E E D B A C K   W I N D O W       *\n";
    cout<<"****************\n\n";

    cout<<"\nThank you for choosing to provide feedback!" <<endl;
    cout<<"Please answer the following questions:\n" <<endl;

    cout<<"------------------------------------------------" <<endl;
    cout<<"How would you rate your overall experience out of 5? ";
    cin >> feedback.overallExperienceRating;
    cin.ignore();

    cout<<"------------------------------------------------" <<endl;
    cout<<"Do you have any suggestions for improvement? (yes/no): ";
    string response;
    cin >> response;
    if (response == "yes" || response == "y")
    {
        cout<<"Please provide your suggestions: ";
        cin.ignore();
        getline(cin, feedback.improvementSuggestions);
    }

    cout<<"------------------------------------------------" <<endl;
    cout<<"Were your concerns addressed adequately? (yes/no): ";
    cin >> response;
    feedback.concernsAddressed = (response == "yes" || response == "y");
    if (!feedback.concernsAddressed)
    {
        cout<<"Please provide the reason for your concerns: ";
        cin.ignore();
        getline(cin, response);
    }

    cout<<"------------------------------------------------" <<endl;
    cout<<"Were you provided with enough information about your condition and treatment options? (yes/no): ";
    cin >> response;
    feedback.enoughInformationProvided = (response == "yes" || response == "y");
    if (!feedback.enoughInformationProvided)
    {
        cout<<"Please consult a doctor for more information." <<endl;
    }

    cout<<"------------------------------------------------" <<endl;
    cout<<"Please provide any additional comments or suggestions (press - if none): ";
    cin.ignore();
    getline(cin, feedback.additionalComments);

}

int main()
{
    // Store the ID of the logged-in patient
    string ID;
    int numYesResponses = 0;
    vector<Patient> patients = readPatients();
    bool loggedIn = login(patients,ID);
    if (!loggedIn)
    {
        cout<<"\nLogin failed. Exiting..." <<endl;
        return 1; // Exit the program with an error code
    }

    for (const auto& patient : patients)
    {
        if (patient.patientId == ID) // <-- Here
        {
            ID = patient.patientId; // This should be corrected to patient.patientId
            break;
        }
    }

    int choice1;
    bool exitProgram = false; // Flag to control program exit
    while (!exitProgram) // Loop until the user chooses to exit
    {
        while(true)
        {
        welcome_window:
        {
        cout<<"\n* - - - - - - - - - - - - - - - - - - - - - - - *"<<endl;
        cout<<"* ------------------- M E N U ------------------- *" <<endl;
        cout<<"\n* - - - - - - - - - - - - - - - - - - - - - - - *"<<endl;
        cout<<"1. Display Personal Information"<<endl;
        cout<<"2. Identify Disease"<<endl;
        cout<<"3. Exit"<<endl;
        cout<<"\n* - - - - - - - - - - - - - - - - - - - - - - - *"<<endl;

        cout<<"\nEnter your choice: ";
        cin >> choice1;
        cin.ignore();

        switch (choice1)
        {
        case 1:
                // Option to display Personal Information
                displayPersonalInformation(ID, patients);
                break;
        case 2:
            //disease identification
            {
                // List of diseases with their symptoms and treatments
        vector<Disease> diseases = {
        {"Fever", {"high temperature", "chills", "sweating", "fatigue"}, {"Paracetamol", "Ibuprofen"}},
        {"Common Cold", {"runny or stuffy nose", "sore throat", "cough", "congestion", "mild body aches or headache"}, {"Acetaminophen", "Ibuprofen", "Decongestants"}},
        {"Influenza", {"fever or feeling feverish/chills", "cough", "sore throat", "runny or stuffy nose", "muscle or body aches", "headaches", "fatigue"}, {"Antiviral drugs", "Analgesics", "Antipyretics"}},
        {"Heart Disease", {"Chest pain", "shortness of breath", "irregular heartbeat"}, {"Enalapril (Renitec - Merck)"," Atenolol (Aten - IPCA)"," Atorvastatin (Lipitor - Pfizer)", "Aspirin (Ecosprin - USV)"}},
        {"Dengue Fever", {"high fever", "severe headache", "joint and muscle pain"}, {"Paracetamol (Crocin - GlaxoSmithKline)", "intravenous fluids for hydration"}},
        {"Chikungunya", {"sudden fever", "joint pain", "muscle pain"}, {"Paracetamol (Crocin - GlaxoSmithKline)", "Ibuprofen (Brufen - Abbott) for pain relief"}},
        {"Malaria", {"fever", "chills", "sweating", "muscle pain"}, {"Chloroquine (Avloclor - AstraZeneca)", "Artemisinin-based combination therapies (ACTs - Various pharmaceuticals)"}},
        {"Tuberculosis (TB)", {"persistent cough", "chest pain", "weight loss"}, {"Isoniazid (INH - Various)", "Rifampicin (Rimactane - Sanofi)", "Ethambutol (Myambutol - Novartis)", "Pyrazinamide"}},
        {"Typhoid Fever", {"sustained fever", "headache", "stomach pain"}, {"Ciprofloxacin (Cipro - Bayer)", "Azithromycin (Zithromax - Pfizer)"}},
        {"Cholera", {"watery diarrhea", "dehydration", "muscle cramps"}, {"Oral rehydration solutions (ORS - Various)", "Azithromycin (Zithromax - Pfizer)"}},
        {"Jaundice (Hepatitis A)", {"yellowing of skin and eyes", "fatigue", "abdominal pain"}, {"Supportive care", "no specific medication for acute hepatitis A"}},
        {"Diarrheal Diseases", {"frequent loose stools", "abdominal cramps", "dehydration"}, {"Oral rehydration solutions (ORS - Various)", "Ciprofloxacin (Cipro - Bayer)", "Azithromycin (Zithromax - Pfizer)"}},
        {"Japanese Encephalitis", {"fever", "headache", "confusion"}, {"Supportive care", "vaccination for prevention"}},
        {"Chronic Obstructive Pulmonary Disease (COPD)", {"chronic cough", "shortness of breath", "wheezing"}, {"Salbutamol (Asthalin - Cipla)", "Salmeterol (Serevent - GlaxoSmithKline)", "Beclomethasone (Beclate - Cipla)", "Fluticasone (Seroflo - Cipla)"}},
        {"Diabetes Mellitus", {"increased thirst", "frequent urination", "weight loss"}, {"Metformin (Glycomet - USV)", "Glimepiride (Amaryl - Sanofi)", "Insulin (Various)"}},
        {"Stroke", {"sudden weakness", "confusion", "difficulty speaking"}, {"Alteplase (Activase - Genentech)", "Aspirin (Ecosprin - USV)", "Warfarin (Coumadin - Bristol-Myers Squibb)"}},
        {"Respiratory Infections (e.g., Pneumonia)", {"fever", "cough", "difficulty breathing"}, {"Amoxicillin (Moxikind - Mankind)", "Azithromycin (Zithromax - Pfizer)", "Ceftriaxone (Rocephin - Roche)"}},
        {"Asthma", {"wheezing", "breathlessness", "chest tightness"}, {"Salbutamol (Asthalin - Cipla)", "Salmeterol (Serevent - GlaxoSmithKline)", "Beclomethasone (Beclate - Cipla)"}}
        };
                unordered_set<string> viewedDiseases;
                int numPredicted = 0;
                int numDetailsDisplayed = 0;
                int numMedicationsDisplayed = 0;

                while (true)
                {
                    vector<Disease> matchingDiseases = identifyDiseases(diseases);
                    numPredicted += matchingDiseases.size();

                    cout<<"\nSuggested diseases based on symptoms:" <<endl;
                    for (size_t i = 0; i < matchingDiseases.size(); ++i)
                    {
                        cout<<i + 1 << ". " << matchingDiseases[i].name <<endl;
                    }

                    if (matchingDiseases.empty())
                    {
                        cout<<"No diseases matched your symptoms. Exiting..." <<endl;
                        break;
                    }
                    char ch;
                    cout<<"Do you want to perform tests to narrow down the diagnosis? (Y/N): ";
                    cin >> ch;
                    cin.ignore();
                    if (toupper(ch) == 'Y')
                    {
                        // List of symptoms for suggesting tests
                        vector<string> symptoms;
                        for (const auto& disease : matchingDiseases)
                        {
                            for (const auto& symptom : disease.symptoms)
                                {
                                    symptoms.push_back(symptom);
                                }
                        }
                        suggestTests(symptoms);
                    }

                    provideDoctorDetails();

                   // Prompt user to choose a disease from the predicted list
                    int index;
                    string choice;
                    while (true)
                        {
                            cout<<"\nEnter the number of the disease to view details (0 to exit, -1 to choose other diseases): ";
                            getline(cin, choice);
                            if (choice == "0")
                            {
                                double bill = calculateBill(numPredicted, numDetailsDisplayed, numMedicationsDisplayed, numYesResponses);

                                // Display the bill to the user
                                cout<<"\n********************"<<endl;
                                cout<<"* THANK YOU FOR USING THE DISEASE IDENTIFYING SYSTEM *" <<endl;
                                cout<<"********************"<<endl;

                                //bill-
                                cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;
                                cout<<"\n\nYour bill for using the system is: $" << fixed << setprecision(2) << bill <<endl;
                                cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;

                                cout<<"*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n";
                                cout<<"*       P A Y M E N T   W I N D O W        *\n";
                                cout<<"*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n";
                                cout<<"Enter C for CASH and O for ONLINE:"<<endl;
                                cout<<"Choose mode of payment: ";
                                char paymentMode;
                                cin >> paymentMode;
                                cin.ignore();

                                if (paymentMode == 'C' || paymentMode == 'c')
                                {
                                    cout<<"\n- - - - - - - - - - - - - - - - - - - - - \n";
                                    cout<<"|                   C A S H                |\n";
                                    cout<<"- - - - - - - - - - - - - - - - - - - - - - \n";
                                    double cash_amt;
                                    cout<<"Enter cash: ";
                                    cin>>cash_amt;
                                    bool flag = calculateChange(bill,cash_amt);
                                    if (flag==true)
                                    {
                                        cout<<"Transaction of Rs." << fixed << setprecision(2) << bill <<" is successful"<<endl;
                                    }
                                }

                               else if(paymentMode == 'O' || paymentMode == 'o')
                                {
                                    cout<<"\n- - - - - - - - - - - - - - - - - - - - - \n";
                                    cout<<"|                 O N L I N E              |\n";
                                    cout<<"- - - - - - - - - - - - - - - - - - - - - \n";
                                    bool flag2 = processOnlinePayment(bill);
                                    if(flag2 == true)
                                    {
                                        cout<<"Transaction of Rs." << fixed << setprecision(2) << bill <<" is successful"<<endl;
                                    }

                                }

                                cout<<"\n****************************************************"<<endl;
                                cout<<"*     E X I T I N G   P A Y M E N T   W I N D O W    *"<<endl;
                                cout<<"\n****************************************************"<<endl;

                                // Provide feedback option
                                char feedbackChoice;
                                cout<<"\nWould you like to provide feedback about your experience? (Y/N): ";
                                cin >> feedbackChoice;
                                if (toupper(feedbackChoice) == 'Y')
                                {
                                    Feedback patientFeedback;
                                    provideFeedback(patientFeedback);
                                    goto welcome_window;
                                }

                                else goto ex_window;

                                // Write the updated patient data back to the file
                                writePatients(patients);
                        }

                        else if (choice == "-1")
                        {
                            cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;
                            cout<<"\n     RETURNING TO DISEASE IDENTIFICATION WINDOW    "<<endl;
                            cout<<"- - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;
                            break; // Allows the user to choose another disease
                        }

                        try
                        {
                            index = stoi(choice) - 1;
                            if (index < 0 || index >= static_cast<int>(matchingDiseases.size()))
                            {
                                throw out_of_range("Invalid index");
                            }

                            const Disease& selectedDisease = matchingDiseases[index];
                            string diseaseName = selectedDisease.name;

                            if (viewedDiseases.count(diseaseName) > 0)
                            {
                                cout<<"You have already viewed details for " << diseaseName << ". Please choose another disease." <<endl;
                                continue;
                            }

                            viewDiseaseDetails(selectedDisease, viewedDiseases);
                            viewedDiseases.insert(diseaseName);
                            numDetailsDisplayed++;

                            if (askYesNoQuestion("Do you want to view treatments for " + diseaseName + "?"))
                            {
                                cout<<"\nTreatments for " << diseaseName << ":" <<endl;
                                for (const auto &treatment : selectedDisease.treatments)
                                {
                                    cout<<"- " << treatment <<endl;
                                }
                                numMedicationsDisplayed++;
                            }
                        }
                catch (const invalid_argument &e)
                {
                    cout<<"Invalid input. Please enter a valid number." <<endl;
                    continue;
                }
                catch (const out_of_range &e)
                {
                    cout<<"Invalid index. Please enter a valid number within the range." <<endl;
                    continue;
                }
                    }
                }
            }

            break;

            case 3:
               ex_window:
                   {
                cout<<"\n** E X I T I N G   T H E   P R O G R A M ***\n"<<endl;
                exitProgram = true; // Set flag to exit the program
                break; // Exit the program
                   }
            default:
                cout<<"Invalid choice. Please enter a number between 1 and 3." <<endl;
        }
            if (exitProgram)
            {
                break;
            }
        }
      }
    }
    return 0;
}
