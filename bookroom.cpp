#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
using namespace std;

struct Date
{
	int year;
	int month;
	int day;
};

struct ReservationInfo
{
	char id[12];
	char name[12];
	char mobile[12];
	int roomType; // 1: superior rooms, 2: deluxe rooms, 3: deluxe twin rooms,
				  // 4: superior suites, 5: deluxe suites
	int numRooms; // number of rooms reserved
	Date checkInDate;
	Date checkOutDate;
};

struct AvailableRooms
{
	Date date;
	int availableRooms[6]; // number of available rooms
						   // availableRooms[ 0 ]: not used
						   // availableRooms[ 1 ]: the number of available superior rooms
						   // availableRooms[ 2 ]: the number of available deluxe rooms
						   // availableRooms[ 3 ]: the number of available deluxe twin rooms
						   // availableRooms[ 4 ]: the number of available superior suites
						   // availableRooms[ 5 ]: the number of available deluxe suites
};

int roomRate[6] = { 0, 5390, 6270, 6270, 6820, 8470 }; // room rate per night for each room type
int numRooms[6] = { 0, 9, 5, 3, 5, 4 }; // total number of rooms for each type
char roomTypeName[6][20] = { "", "Superior Room", "Deluxe Room", "Deluxe Twin Room",
"Superior Suite", "Deluxe Suite" };
int days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // number of days in each month

int enterChoice();

void makeReservation();

// put the current date into currentDate
void computeCurrentDate(Date &currentDate);

// input check in date and check out date by user, and put them into checkInDate and checkOutDate, respectively.
void inputDates(const Date &currentDate, Date &checkInDate, Date &checkOutDate);

// load all available rooms information from the file Available Rooms.dat
void loadAvailableRooms(AvailableRooms availableRooms[], int &numRecords);

// returns a value of 0, -1 or 1 if date1 is equal to, less than or greater than date2, respectively.
int compareDates(const Date &date1, const Date &date2);

void initializeAvailableRooms(AvailableRooms availableRooms[], const Date &currentDate, int &numRecords);

void adjustAvailableRooms(AvailableRooms availableRooms[], const Date &currentDate, int &numRecords);

// minNumRooms[ k ] = the minimum between availableRooms[ begin .. end - 1 ].availableRooms[ k ], k = 1, ..., 5,
// where availableRooms[ begin ].date == checkInDate and availableRooms[ end ].date, checkOutDate.
int compMinNumRooms(const Date &checkInDate, const Date &checkOutDate,
	AvailableRooms availableRooms[], int numRecords, int minNumRooms[]);

// return the minimum between availableRooms[ begin .. end - 1 ].availableRooms[ roomType ],
// where availableRooms[ begin ].date == checkInDate and availableRooms[ end ].date, checkOutDate.
int compMinNumRooms(const Date &checkInDate, const Date &checkOutDate,
	AvailableRooms availableRooms[], int numRecords, int roomType);

void display(const AvailableRooms &availableRoom);

// return true if and only if id is a legal ID card number
bool legal(char id[]);

void display(const ReservationInfo &reservation, time_t numNights);

// save reservation into the end of the file Reservations.dat
void saveReservationInfo(const ReservationInfo &reservation);

// save all available rooms information into the file Available Rooms.dat
void saveAvailableRooms(AvailableRooms availableRooms[], const int numRecords);
void reservationInquiry();

int main()
{
	cout << "Evergreen Laurel Hotel Online Reservation System\n";

	int choice;

	while ((choice = enterChoice()) != 3)
	{
		switch (choice)
		{
		case 1:
			makeReservation(); // Making Reservation
			break;
		case 2:
			reservationInquiry(); // Reservation Inquiry
			break;
		default:
			cerr << "\nInput Error!" << endl;
		}
	}

	cout << endl;

	system("pause");
}

int enterChoice()
{
	cout << "\nInput your choice：\n"
		<< "1. Making Reservation\n"
		<< "2. Reservation Inquiry\n"
		<< "3. end program\n? ";

	int menuChoice;
	cin >> menuChoice;
	return menuChoice;
}

void makeReservation()
{
	int numrecord = 0;
	AvailableRooms available[184] = {};
	Date date = {};
	Date checkin = {};
	Date checkout = {};
	computeCurrentDate(date);
	inputDates(date, checkin, checkout);
	cout << "The number of available rooms of each room type: " << endl;
	cout << "      Date   Superior Room   Deluxe Room   Deluxe Twin Room   Superior Suite   Deluxe Suite" << endl;
	loadAvailableRooms(available, numrecord);

	if (compareDates(available[numrecord - 2].date, date) == -1) {
		//cout << "do this" << endl;
		initializeAvailableRooms(available, date, numrecord);
	}
	else {
		//cout << "do" << endl;
		if (compareDates(available[1].date, date) == -1) {
			adjustAvailableRooms(available, date, numrecord);
		}
	}
	

	int minNumRooms[6] = {};//就是存每個房型最多可以訂的房間數
	for (int i = 0; i < numrecord; i++) {
		if ((available[i].date.year == checkin.year) && (available[i].date.month == checkin.month) && (available[i].date.day == checkin.day)) {
			for (int j = i; j < numrecord; j++) {
				if ((available[j].date.year == checkout.year) && (available[j].date.month == checkout.month) && (available[j].date.day == checkout.day)) {
					for (int k = i; k < j; k++) {
						display(available[k]);
					}
				}
			}
		}
	}

	ReservationInfo reservation;
	cout << "Select Room Type：" << endl;
	compMinNumRooms(checkin, checkout, available, numrecord, minNumRooms);

	do {
		for (int i = 1; i < 6; i++) {
			if (minNumRooms[i] != 0) {
				cout << i << ". ";
				for (int j = 0; j < 20; j++) {
					cout << roomTypeName[i][j];
				}
				cout << endl;
			}
		}
		cin >> reservation.roomType;
	} while (reservation.roomType < 1 || reservation.roomType>6);

	do {
		cout << "Number of rooms ( " << minNumRooms[reservation.roomType] << " rooms available ):";
		cin >> reservation.numRooms;
	} while (reservation.numRooms > minNumRooms[reservation.roomType]);
	cout << "ID Number：";
	do {
		cin.getline(reservation.id, 12);
	} while (legal(reservation.id)==false);
	cout << "Name：";
	cin.getline(reservation.name, 12);
	cout << "Mobile Phone：";
	cin.getline(reservation.mobile, 12);

	reservation.checkInDate.year = checkin.year;
	reservation.checkInDate.month = checkin.month;
	reservation.checkInDate.day = checkin.day;
	reservation.checkOutDate.year = checkout.year;
	reservation.checkOutDate.month = checkout.month;
	reservation.checkOutDate.day = checkout.day;

	saveReservationInfo(reservation);
}

// returns a value of 0, -1 or 1 if date1 is equal to, less than or greater than date2, respectively.
void computeCurrentDate(Date &currentDate)
{
	tm structuredTime;
	time_t rawtime = time(0);
	localtime_s(&structuredTime, &rawtime);

	currentDate.year = structuredTime.tm_year + 1900;
	currentDate.month = structuredTime.tm_mon + 1;
	currentDate.day = structuredTime.tm_mday;
	cout << "The current date is: " << structuredTime.tm_year + 1900 << "-" << structuredTime.tm_mon + 1 << "-" << structuredTime.tm_mday << " "
		<< structuredTime.tm_hour << ":" << structuredTime.tm_min << ":" << structuredTime.tm_sec << endl;
}

// input check in date and check out date by user, and put them into checkInDate and checkOutDate, respectively.
void inputDates(const Date &currentDate, Date &checkInDate, Date &checkOutDate)
{
	int month, day;
	int out_month, out_day;
	//-----------------------------------------check in------------------------------------------------
	do {
		cout << "Please Input Check In Date" << endl;
		cout << "Month: " << endl;
		cout << "1. " << currentDate.year << " - " << currentDate.month << endl;
		for (int i = 1; i < 6; i++) {
			if (currentDate.month + i > 12) {
				cout << i + 1 << ". " << currentDate.year + 1 << " - " << currentDate.month + i - 12 << endl;
			}
			else {
				cout << i + 1 << ". " << currentDate.year << " - " << currentDate.month + i << endl;
			}
		}
		cin >> month;
	} while (month < 1 || month>6);
	if (month == 1)
		checkInDate.month = currentDate.month;
	else
		checkInDate.month = month - 1;
	//input day
	if (month == 1) {
		checkInDate.year = currentDate.year;
		do {
			cout << "Day (" << currentDate.day << "~" << days[currentDate.month] << ") : ";
			cin >> day;
		} while (day<currentDate.day || day>days[currentDate.month]);
	}
	else {
		do {
			if (currentDate.month + month > 12) {
				checkInDate.year = currentDate.year + 1;
			}
			else {
				checkInDate.year = currentDate.year;
			}
			cout << "Day ( 1 ~ " << days[checkInDate.month] << ") : ";
			cin >> day;
		} while (day<1 || day>days[checkInDate.month]);
	}
	checkInDate.day = day;
	//-----------------------------------check out-------------------------------------------- 
	// 跨月還沒加喔!!!!
	do {
		cout << "Please Input Check Out Date" << endl;
		cout << "Month: " << endl;
		cout << "1. " << checkInDate.year << " - " << checkInDate.month << endl;
		for (int i = 1; i < 6; i++) {
			if (checkInDate.month + i > 12) {
				cout << i + 1 << ". " << checkInDate.year + 1 << " - " << checkInDate.month + i - 12 << endl;
			}
			else {
				cout << i + 1 << ". " << checkInDate.year << " - " << checkInDate.month + i << endl;
			}
		}
		cin >> out_month;
	} while (out_month < 1 || out_month>6);
	if (checkInDate.month + out_month - 1 > 12)
		checkOutDate.year = checkInDate.year + 1;
	else
		checkOutDate.year = checkInDate.year;

	if (out_month == 1)
		checkOutDate.month = checkInDate.month;
	else
		checkOutDate.month = out_month - 1;
	//cout << " out " << checkOutDate.month<< endl;
	//    in / out / current same
	if (checkInDate.month == checkOutDate.month) {
		do {
			cout << "Day : (" << checkInDate.day + 1 << "~" << days[checkOutDate.month] << ") :";
			cin >> out_day;
			checkOutDate.day = out_day;
		} while (checkOutDate.day<checkInDate.day || checkOutDate.day>days[checkOutDate.month]);
	}
	else {
		do {
			cout << "Day : (1 ~" << days[checkOutDate.month] << ") :";
			cin >> out_day;
			checkOutDate.day = out_day;
		} while (checkOutDate.day<1 || checkOutDate.day>days[checkOutDate.month]);
	}

	cout << "Check in date: " << checkInDate.year << " - " << checkInDate.month << " - " << checkInDate.day << endl;
	cout << "Check out date: " << checkOutDate.year << " - " << checkOutDate.month << " - " << checkOutDate.day << endl;
}

// load all available rooms information from the file Available Rooms.dat
void loadAvailableRooms(AvailableRooms availableRooms[], int &numRecords)
{

	ifstream file("Available Rooms.dat", ios::binary);
	numRecords++;
	while (!file.eof())
	{
		file.read(reinterpret_cast<char*>(&availableRooms[numRecords]), sizeof(AvailableRooms));
		numRecords++;
	}


	file.close();
}

int compareDates(const Date &date1, const Date &date2)
{
	if (date1.year > date2.year)
		return 1;
	if (date1.year < date2.year)
		return -1;

	if (date1.month > date2.month)
		return 1;
	if (date1.month < date2.month)
		return -1;

	if (date1.day > date2.day)
		return 1;
	if (date1.day < date2.day)
		return -1;

	return 0;
}

void initializeAvailableRooms(AvailableRooms availableRooms[], const Date &currentDate, int &numRecords)
{
	//initial
	availableRooms[0].date.year = currentDate.year;
	availableRooms[0].date.month = currentDate.month;
	availableRooms[0].date.day = currentDate.day;

	for (int i = 1; i < numRecords; i++) {
		availableRooms[i].date.day = availableRooms[i - 1].date.day + 1;
		availableRooms[i].date.month = availableRooms[i - 1].date.month;
		availableRooms[i].date.year = availableRooms[i - 1].date.year;
		//31day
		if (availableRooms[i].date.month == 1 || availableRooms[i].date.month == 3 || availableRooms[i].date.month == 5
			|| availableRooms[i].date.month == 7 || availableRooms[i].date.month == 8 || availableRooms[i].date.month == 10 || availableRooms[i].date.month == 12) {
			if (availableRooms[i].date.day > 31) {
				availableRooms[i].date.day -= 31;
				availableRooms[i].date.month++;
			}
			if (availableRooms[i].date.month > 12) {
				availableRooms[i].date.month -= 12;
				availableRooms[i].date.year++;
			}
		}
		//28day
		else if (availableRooms[i].date.month == 2) {
			if (availableRooms[i].date.day > 28) {
				availableRooms[i].date.day -= 28;
				availableRooms[i].date.month++;
			}
			if (availableRooms[i].date.month > 12) {
				availableRooms[i].date.month -= 12;
				availableRooms[i].date.year++;
			}
		}
		//30day
		else {
			if (availableRooms[i].date.day > 30) {
				availableRooms[i].date.day -= 30;
				availableRooms[i].date.month++;
			}
			if (availableRooms[i].date.month > 12) {
				availableRooms[i].date.month -= 12;
				availableRooms[i].date.year++;
			}
		}
	}
}

void adjustAvailableRooms(AvailableRooms availableRooms[], const Date &currentDate, int &numRecords)
{
	//cout << currentDate.year << "**" << currentDate.month << "**" << currentDate.day << endl;
	int bigger = 0;
	//cout << "start" << endl;
	for (int i = 1; i < numRecords - 2; i++) {
		if (compareDates(availableRooms[i].date, currentDate) == -1) {
			//cout << availableRooms[i].date.year << " - " << availableRooms[i].date.month << " - " << availableRooms[i].date.day << endl;
			bigger++;
		}
	}

	for (int i = 1; i < numRecords - 2 - bigger; i++) {
		availableRooms[i].date.year = availableRooms[i + bigger].date.year;
		availableRooms[i].date.month = availableRooms[i + bigger].date.month;
		availableRooms[i].date.day = availableRooms[i + bigger].date.day;
		for (int j = 0; j < 6; j++) {
			availableRooms[i].availableRooms[j] = availableRooms[i + bigger].availableRooms[j];
		}
	}
	for (int i = numRecords - 1; i >= numRecords - 2 - bigger; i--) {
		for (int j = 0; j < 6; j++) {
			availableRooms[i].availableRooms[j] = numRooms[j];
		}
	}
}

int compMinNumRooms(const Date &checkInDate, const Date &checkOutDate,//每個房型裡的最大值
	AvailableRooms availableRooms[], int numRecords, int minNumRooms[])
{
	int minMax = 0;
	for (int i = 1; i <= 5; i++)
	{
		minNumRooms[i] = compMinNumRooms(checkInDate, checkOutDate, availableRooms, numRecords, i);
		if (minMax < minNumRooms[i])
			minMax = minNumRooms[i];
	}

	return minMax;
}

int compMinNumRooms(const Date &checkInDate, const Date &checkOutDate,//區間內每個房型的最小值
	AvailableRooms availableRooms[], int numRecords, int roomType)
{
	int checkInday;
	int checkOutday;
	for (int i = 0; i < numRecords; i++)
	{
		if (compareDates(availableRooms[i].date, checkInDate) == 0)
		{
			checkInday = i; //availableRooms[checkInday]
			while (compareDates(availableRooms[i].date, checkOutDate) == -1)
			{
				checkOutday = i;
				i++;
			}
			/*cout << checkInday << endl;
			cout << checkOutday << endl;*/
			break;
		}
	}
	int number = 10;
	for (int i = checkInday; compareDates(availableRooms[i].date, checkOutDate) == -1; i++)
	{
		if (availableRooms[i].availableRooms[roomType] <= number)
			number = availableRooms[i].availableRooms[roomType];
		if (availableRooms[i].availableRooms[roomType] == 0)
		{
			number = 0;
			break;
		}
	}
	return number;
}

void display(const AvailableRooms &availableRoom)
{
	cout << availableRoom.date.year << "-"
		<< setw(2) << setfill('0') << availableRoom.date.month << "-"
		<< setw(2) << availableRoom.date.day
		<< setw(16) << setfill(' ') << availableRoom.availableRooms[1]
		<< setw(14) << availableRoom.availableRooms[2]
		<< setw(19) << availableRoom.availableRooms[3]
		<< setw(17) << availableRoom.availableRooms[4]
		<< setw(15) << availableRoom.availableRooms[5] << endl;
}

bool legal(char id[])
{
	int length = strlen(id);

	if (length != 10)
		return false;
	else {
		int firstChar[26] = { 10, 11, 12, 13, 14, 15, 16, 17, 34, 18,
			19, 20, 21, 22, 35, 23, 24, 25, 26, 27,
			28, 29, 32, 30, 31, 33 };
		int leading;

		if (id[0] >= 'a'&&id[0] <= 'z')
			leading = firstChar[id[0] - 'a'];
		if (id[0] >= 'A'&&id[0] <= 'Z')
			leading = firstChar[id[0] - 'A'];

		int sum = (leading % 10) * 9 + (leading / 10);

		for (int i = 1; i <= 8; i++)
			sum += (id[i] - '0')*(9 - i);
		sum = sum + (id[9] - '0');
		if (sum % 10 == 0)
			return true;
		else
			return false;
	}
}

void display(const ReservationInfo &reservation, time_t numNights)
{
	cout << setw(8) << setfill(' ') << reservation.name
		<< setw(7) << reservation.numRooms * roomRate[reservation.roomType] * numNights
		<< setw(13) << reservation.mobile
		<< setw(19) << roomTypeName[reservation.roomType]
		<< setw(18) << reservation.numRooms
		<< setw(10) << reservation.checkInDate.year << "-"
		<< setw(2) << setfill('0') << reservation.checkInDate.month << "-"
		<< setw(2) << reservation.checkInDate.day
		<< setw(11) << setfill(' ') << reservation.checkOutDate.year << "-"
		<< setw(2) << setfill('0') << reservation.checkOutDate.month << "-"
		<< setw(2) << reservation.checkOutDate.day << endl;
}

void saveReservationInfo(const ReservationInfo &reservation)
{
	cout << "Your reservations:" << endl;
	cout << "    Name   Fare       Mobile          Room type   Number of rooms   Check in date   Check out date" << endl;
	cout << reservation.name << "  " << reservation.numRooms*roomRate[reservation.roomType] << "  ";
	for (int i = 0; i < 20; i++) {
		cout << reservation.mobile[i];
	}
	for (int i = 0; i < 20; i++)
		cout << roomTypeName[reservation.roomType][i];
	cout << "" << reservation.numRooms << " " << reservation.checkInDate.year << "-" << reservation.checkInDate.month << "-" << reservation.checkInDate.day << "       ";
	cout << reservation.checkOutDate.year << "-" << reservation.checkOutDate.month << "-" << reservation.checkOutDate.day;

	ofstream outClientFile("Reservations.dat.dat", ios::binary | ios::app);
	outClientFile.write(reinterpret_cast<const char*>(&reservation),sizeof(ReservationInfo));

	outClientFile.close();

}

void saveAvailableRooms(AvailableRooms availableRooms[], const int numRecords)
{
	ofstream outClientFile("Available Rooms.dat", ios::binary | ios::app);
	for (int i = 0; i < numRecords; i++) {
		outClientFile.write(reinterpret_cast<char*>(&availableRooms[i]), sizeof(availableRooms[i]));
	}

	outClientFile.close();
}

void reservationInquiry()
{
	ReservationInfo reservation;
	ifstream infile("Reservations.dat.dat", ios::binary);

	if (!infile) {
		ofstream infile("Reservations.dat.dat", ios::binary);
	}
	while (!infile.eof())
	{
		infile.read(reinterpret_cast<char*>(&reservation), sizeof(ReservationInfo));
		cout << "\t" << reservation.name << "  " << reservation.numRooms*roomRate[reservation.roomType] << "  ";
		for (int i = 0; i < 20; i++) {
			cout << reservation.mobile[i];
		}
		for (int i = 0; i < 20; i++)
			cout << roomTypeName[reservation.roomType][i];
		cout << "" << reservation.numRooms << " " << reservation.checkInDate.year << "-" << reservation.checkInDate.month << "-" << reservation.checkInDate.day << "       ";
		cout << reservation.checkOutDate.year << "-" << reservation.checkOutDate.month << "-" << reservation.checkOutDate.day;
	}
	infile.close();
}