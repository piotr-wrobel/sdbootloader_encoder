#include <iostream>
#include <fstream>
#include <cstdlib>
#include "main.h"
using namespace std;


string getCmdOption(string name, string defaut, const int argc, const char *const * argv)
{
  string res = "0";
  
  if (argc > 0) {
    int k = 0;
    while (k < argc && argv[k]!=name) ++k;
    res = (k++==argc?defaut:(k==argc?argv[--k]:(argv[k][0]==(int)'-'?argv[--k]:argv[k])));
  } else res = defaut;
  
  return res;
}


int main( int argc, char* argv[] )
{
	int silent,no_ofile;

	silent = (getCmdOption("-s","0", argc, argv )=="-s"?1:0);
	no_ofile = (getCmdOption("-no","0", argc, argv )=="-no"?1:0);

	
	if(!silent)	
		cout << endl
      	<< "SDBootloader Encoder version 1.0"
		<< "  ( code by (-)pvg )" << endl << endl;
	  
	if( argc == 1)
	{
		cout << "Uzycie: " << argv[0] << " [-s][-no] -i <input_ihex_filename> [ -o <output_ihex_filename>]"<< endl 
		<< "-s : silent - brak danych informacyjnych na standardowym wyjsciu" << endl 
		<< "-no : no output file - dane wyjsciowe pojawia sie na standardowym wyjsciu" << endl;
		return 1;
	}

	string infile  = getCmdOption("-i","0", argc, argv );
	if (infile == "0" || infile == "-i") { 
	  cerr << "Error: Podaj jako parametr nazwe pliku wejsciowego -i <input_ihex_filename>" << endl; 
	  return 1;
	}
	string plik=infile.substr(infile.find_last_of("/\\")+1);
	string nazwa_pliku=plik.substr(0,plik.find_last_of("."));
	string rozszerzenie=plik.substr(plik.find_last_of("."));
	string ihexfile=".hex";
	
	if (ihexfile.compare(rozszerzenie) != 0)
	{
		cerr << "Error: Jako plik wejsciowy musisz podac plik *.hex"<<endl;
		return 1;
	}

	string ofile = getCmdOption("-o","0", argc, argv );
	if (ofile == "0" || ofile == "-o")
		ofile=nazwa_pliku+".encoded.hex";

	if(!silent)
		cout << "Plik wejsciowy: \"" << infile <<"\" , wyjsciowy: \"" << ofile << "\"" << endl;



	ifstream inputfile(infile.data());
	if(!inputfile.good())
	{
		cerr << "Error: Nie mozna otworzyc pliku wejsciowego \"" << infile << "\" do odczytu!" << endl;
		inputfile.close();
		return 1;
	}
	
	char znak,bufor[255]; 

	string rekord="", nowy_rekord="";
	int dlugosc_rekordu, adres_rekordu, adres_biezacy;
	int	typ_rekordu, bajt_danych, nowy_bajt, klucz;
	int suma_kontrolna,nowa_suma_kontrolna,odczytana_suma_kontrolna,indeks=0;
	size_t skopiowano;
	ofstream outfile(ofile.data());
	if(!no_ofile)
	{
		//ofstream outfile(ofile.data());
		if(!outfile.good())
		{
			cerr << "Error: Nie mozna otworzyæ pliku " << ofile << " do zapisu!" << endl;
			outfile.close();
			return 1;
		}
	}

	
	for(znak = inputfile.get();!inputfile.eof();znak = inputfile.get())
	{
		if(znak!=':')
			rekord+=znak;
		else if(rekord.length()>0)
		{
			//if(!silent)
				//cout << "rekord zrodlowy: :" << rekord << endl;
			skopiowano=rekord.copy(bufor,2,0);
			if(skopiowano==2)
			{
				bufor[skopiowano]='\0';
				dlugosc_rekordu=strtol(bufor,NULL,16);
			}else
			{	cerr << "Error: Nie moge odczytac dlugosci rekordu: " << rekord << skopiowano <<endl;
				return 1;
			}
			skopiowano=rekord.copy(bufor,4,2);
			if(skopiowano==4)
			{
				bufor[skopiowano]='\0';
				adres_rekordu=strtol(bufor,NULL,16);
			}else
			{	cerr << "Error: Nie moge odczytac adresu rekordu: " << rekord << endl;
				return 1;
			}			
			skopiowano=rekord.copy(bufor,2,6);
			if(skopiowano==2)
			{
				bufor[skopiowano]='\0';
				typ_rekordu=strtol(bufor,NULL,16);
			}else
			{	
				cerr << "Error: Nie moge odczytac typu rekordu: " << rekord << endl;
				return 1;
			}			
			
			suma_kontrolna=dlugosc_rekordu+(adres_rekordu&0x00FF)+(adres_rekordu>>8)+typ_rekordu; //baza sumy kontrolnej
			nowa_suma_kontrolna=suma_kontrolna;
			if(!typ_rekordu)
			{
				for(indeks=0;indeks<dlugosc_rekordu;indeks++)
				{
					skopiowano=rekord.copy(bufor,2,8+(indeks*2));
					if(skopiowano==2)
					{
					 	bufor[skopiowano]='\0';
						bajt_danych=strtol(bufor,NULL,16);
						suma_kontrolna+=bajt_danych;
						
						adres_biezacy=adres_rekordu+indeks;
						
						if((adres_biezacy & 0xFF)==CODE_MASK)
						{
							klucz=(adres_biezacy>>4)&0xFF;
							nowy_bajt=(bajt_danych+klucz)&0xFF;
						}else
							nowy_bajt=bajt_danych;
						
						nowa_suma_kontrolna+=nowy_bajt;
						skopiowano=sprintf(bufor,"%02X",nowy_bajt);
						bufor[skopiowano]='\0';
						nowy_rekord+=bufor; //Tu tworzymy nowy rekord, same dane 
#ifdef DEBUG						
						skopiowano=sprintf(bufor,"adres: 0x%04X -> 0x%02X -> 0x%02X",adres_biezacy,bajt_danych,nowy_bajt);
						bufor[skopiowano]='\0';
						cout << bufor << endl;
#endif						
					}else
					{
						cerr << "Error: Nie moge odczytac typu bajtu danych o indeksie: " << indeks 
						<< " z rekordu: " << rekord <<endl;
						return 1;						
					}
				}
			}
			skopiowano=rekord.copy(bufor,2,8+(indeks*2));
			if(skopiowano==2)
			{
				bufor[skopiowano]='\0';
				odczytana_suma_kontrolna=strtol(bufor,NULL,16);
			}else
			{	
				cerr << "Error: Nie moge odczytac sumy kontrolnej rekordu: " << rekord << endl;
				return 1;
			}			

			suma_kontrolna=(0x100-(suma_kontrolna&0xFF))&0xFF;
			nowa_suma_kontrolna=(0x100-(nowa_suma_kontrolna&0xFF))&0xFF;
			skopiowano=sprintf(bufor,"%02X",nowa_suma_kontrolna);
			bufor[skopiowano]='\0';
			nowy_rekord+=bufor; //Doklejamy na koniec sume kontrolna
			skopiowano=sprintf(bufor,":%02X%04X%02X",dlugosc_rekordu,adres_rekordu,typ_rekordu);
			bufor[skopiowano]='\0';
			nowy_rekord=bufor+nowy_rekord;
			if(!no_ofile)
			{
				outfile << nowy_rekord << endl;
			} else
				cout << nowy_rekord << endl;
			//if(!silent)
				//cout << "nowy rekord:     " << nowy_rekord << endl <<endl;
			
#ifdef DEBUG			
			
			skopiowano=sprintf(bufor,"0x%02X",dlugosc_rekordu);
			bufor[skopiowano]='\0';
			cout << "dlugosc: " << bufor;

			skopiowano=sprintf(bufor,"0x%04X",adres_rekordu);
			bufor[skopiowano]='\0';
			cout <<  " adres: " << bufor;
			
			skopiowano=sprintf(bufor,"0x%02X",typ_rekordu);
			bufor[skopiowano]='\0';
			cout << " typ: " << bufor;
			
			skopiowano=sprintf(bufor,"0x%02X",odczytana_suma_kontrolna);
			bufor[skopiowano]='\0';
			cout <<  " suma_kontrolna: " << bufor;
			
			cout << endl;
#endif			
			if(suma_kontrolna!=odczytana_suma_kontrolna)
			{
				skopiowano=sprintf(bufor,"%X",suma_kontrolna);
				bufor[skopiowano]='\0';				
				cerr << "Error: Suma kontrolna rekordu jest nieprawidlowa: " << rekord 
				<< " wyliczona suma: " << bufor << endl;
				return 1;				
			}
			rekord="";nowy_rekord="";
		}
	}
	if(!no_ofile)
	{
		outfile << ":00000001FF" << endl;
		outfile.close();
	}else
		cout << ":00000001FF" << endl;
	
	if(!silent && !no_ofile)
		cout << "Wynik zapisany do pliku " << ofile << endl;
	return 0;

}
