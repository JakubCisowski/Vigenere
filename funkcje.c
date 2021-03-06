#include "funkcje.h"

bool wczytajPrzelaczniki(int ileArgumentow, char* argumenty[], char** kluczSzyfrujacy, char** plikWejsciowy, char** plikWyjsciowy, bool* szyfrowanie, bool* deszyfrowanie)
{
	// Zmienne pomocnicze do sprawdzenia czy podano wszystkie wymagane przelaczniki przy wlaczeniu programu
	bool jestKlucz = false, jestWej = false, jestWyj = false, jestSzyfr = false, jestDeszyfr = false;

	// Iterowanie po kazdym z przelacznikow
	for (int i = 1; i < ileArgumentow; i++)
	{
		// Przelacznik pliku wejsciowego
		if (strcmp(argumenty[i], "-i") == 0)
		{
			// Sprawdzenie czy istnieje nastepny argument
			if (i + 1 == ileArgumentow)
			{
				printf("Nie podano nazwy pliku wejsciowego po przelaczniku -i.\n");
				return false;
			}

			// Sprawdzanie poprawnosci pliku wejsciowego
			FILE* file;
			if ((file = fopen(argumenty[i+1], "r")) != NULL)
			{
				jestWej = true;
				*plikWejsciowy = argumenty[++i];
				fclose(file);
			}
			else
			{
				printf("Podanego pliku wejsciowego nie udalo sie otworzyc (pamietaj o dopisaniu .txt).\n");
				return false;
			}
		}

		// Przelacznik pliku wyjsciowego
		if(strcmp(argumenty[i], "-o") == 0)
		{
			// Sprawdzenie czy istnieje nastepny argument
			if (i + 1 == ileArgumentow)
			{
				printf("Nie podano nazwy pliku wyjsciowego po przelaczniku -o.\n");
				return false;
			}
			
			// Sprawdzenie poprawnosci pliku wyjsciowego
			FILE* file;
			if ((file = fopen(argumenty[i + 1], "a")) != NULL)
			{
				printf("<Plik wyjsciowy utworzony, upewnij sie czy stworzyles go w formacie .txt>\n");
				jestWyj = true;
				*plikWyjsciowy = argumenty[++i];
				fclose(file);
			}
			else
			{
				printf("Podanego pliku wyjsciowego nie udalo sie utworzyc (pamietaj o dopisaniu .txt).\n");
				return false;
			}
		}

		// Przelacznik klucza szyfrujacego
		if (strcmp(argumenty[i], "-k") == 0)
		{
			// Sprawdzenie czy istnieje nastepny argument
			if (i + 1 == ileArgumentow)
			{
				printf("Nie podano klucza szyfrujacego po przelaczniku -k.\n");
				return false;
			}

			//Sprawdzenie poprawnosci klucza szyfrujacego
			if (sprawdzKlucz(argumenty[i + 1]))
			{
				jestKlucz = true;
				*kluczSzyfrujacy = argumenty[++i];
			}
			else
			{
				printf("Podany klucz szyfrujacy jest niepoprawny, pamietaj ze moze skladac sie jedynie z malych oraz wielkich liter alfabetu lacinskiego.\n");
				return false;
			}
			
		}

		// Przelacznik szyfrowania
		if (strcmp(argumenty[i], "-e") == 0)
		{
			if (jestDeszyfr)
			{
				printf("Nie mozna jednoczesnie szyfrowac oraz deszyfrowac, nastepnym razem wybierz jedna opcje.\n");
				return false;
			}

			jestSzyfr = true;
			*szyfrowanie = true;
		}

		// Przelacznik deszyfrowania
		if (strcmp(argumenty[i], "-d") == 0)
		{
			if (jestSzyfr)
			{
				printf("Nie mozna jednoczesnie szyfrowac oraz deszyfrowac, nastepnym razem wybierz jedna opcje.\n");
				return false;
			}

			jestDeszyfr = true;
			*deszyfrowanie = true;
		}

	}

	// Sprawdzenie czy wszystkie potrzebne przelaczniki zostaly podane
	if (!jestWej || !jestWyj || !jestKlucz || (!jestDeszyfr && !jestSzyfr))
	{
		printf("Nie podano wszystkich wymaganych przelacznikow, wymagane: -i -o -k oraz jeden z dwoch trybow: -e lub -d.\n");
		return false;
	}

	return true;
}

int sprawdzKlucz(char* klucz)
{
	int i, x = 0, p;
	p = strlen(klucz);

	for (i = 0; i < p; i++)
		if ((klucz[i] >= 'a' && klucz[i] <= 'z') || (klucz[i] >= 'A' && klucz[i] <= 'Z'))
		{
			continue;
		}
		else return 0;

	return 1;
}

Bufor* wczytajDane(char* plikWejsciowy)
{
	Bufor* glowa = NULL;

	FILE* plik = fopen(plikWejsciowy, "r");
	char czescTekstu[10];

	while (fgets(czescTekstu, sizeof(czescTekstu), plik))
	{
		//printf("\n%s",czescTekstu);
		glowa = wstawNaKoniec(glowa, czescTekstu);
	}

	fclose(plik);

	return glowa;
}

void zapiszDane(Bufor* glowa, char* plikWyjsciowy)
{
	FILE* plik = fopen(plikWyjsciowy, "w");

	// Iterujemy po czesciach tekstu w buforze
	while(glowa != NULL)
	{
		fprintf(plik, glowa->wartosc);
		glowa = glowa->next;
	}

	fclose(plik);
}

Bufor* nowyElement(char* wartosc)
{
	Bufor* nowyElement = (Bufor*)malloc(sizeof(Bufor));
	nowyElement->wartosc = _strdup(wartosc);
	nowyElement->next = NULL;
	return nowyElement;
}

Bufor* wstawNaKoniec(Bufor* glowa, char* wartosc)
{
	if (glowa == NULL)
		return nowyElement(wartosc);

	else
		glowa->next = wstawNaKoniec(glowa->next, wartosc);
	return glowa;
}

void Viegenere(Bufor* glowa, char* klucz, bool szyfrowanie, bool deszyfrowanie)
{
	// Konwerujemy klucz w postaci liter na klucz z liczbami w celu ulatwienia szyfrowania
	int* kluczLiczbowy = (int*)malloc(strlen(klucz) * sizeof(int));
	konwertujKlucz(kluczLiczbowy, klucz);

	// Bedziemy przesuwac sie po kluczu, wiec potrzebny indeks, na starcie pierwszy element
	int indeksWKluczu = 0;

	Bufor* temp = glowa;

	while (temp->next != NULL)
	{
		temp->wartosc = szyfruj(temp->wartosc, &indeksWKluczu, kluczLiczbowy, szyfrowanie, deszyfrowanie);
		temp = temp->next;
	}

	// Ostani element
	temp->wartosc = szyfruj(temp->wartosc, &indeksWKluczu, kluczLiczbowy, szyfrowanie, deszyfrowanie);
}

char* szyfruj(char* napis, int* indeks, int* klucz, bool szyfrowanie, bool deszyfrowanie)
{


	char* nowy = (char*)malloc((strlen(napis)+1) * sizeof(char));

	for (int i = 0; i < strlen(napis); i++)
	{

		// Kod ascii obecnego znaku
		int ascii = napis[i];

		if (szyfrowanie == true)
		{
			if (ascii >= 65 && ascii <= 90)
			{
				if (ascii + klucz[*indeks] <= 90)
				{
					nowy[i] = ascii + klucz[*indeks];
				}
				else if(ascii + klucz[*indeks] > 90)
				{
					nowy[i] = ascii + klucz[*indeks] - 26;
				}
			}
			else if (ascii >= 97 && ascii <= 122)
			{
				if (ascii + klucz[*indeks] <= 122)
				{
					nowy[i] = ascii + klucz[*indeks];
				}
				else if (ascii + klucz[*indeks] > 122)
				{
					nowy[i] = ascii + klucz[*indeks] - 26;
				}
			}
			else
			{
				nowy[i] = napis[i];
			}
		}
		else if (deszyfrowanie == true)
		{
			if (ascii >= 65 && ascii <= 90)
			{
				if (ascii - klucz[*indeks] >= 65)
				{
					nowy[i] = ascii - klucz[*indeks];
				}
				else if (ascii - klucz[*indeks] < 65)
				{
					nowy[i] = ascii - klucz[*indeks] + 26;
				}
			}
			else if (ascii >= 97 && ascii <= 122)
			{
				if (ascii - klucz[*indeks] >= 97)
				{
					nowy[i] = ascii - klucz[*indeks];
				}
				else if (ascii - klucz[*indeks] < 97)
				{
					nowy[i] = ascii - klucz[*indeks] + 26;
				}
			}
			else
			{
				nowy[i] = napis[i];
			}
		}

		// Przesuniecie indeksu
		if (*indeks == sizeof(klucz))
			*indeks = 0;
		else
			*indeks += 1;

	}
	// Dodanie null na koniec napisu
	nowy[strlen(napis)] = '\0';

	return nowy;
}

void konwertujKlucz(int* kluczLiczbowy, char* klucz)
{
	for (int i = 0; i < strlen(klucz); i++)
	{
		// Kod ascii obecnego znaku
		int ascii = klucz[i];

		// Zamieniamy na liczby, A to przesuniecie o 0 wiec:
		if (ascii >= 65 && ascii <= 90)
		{
			kluczLiczbowy[i] = ascii - 65;
		}
		else
		{
			kluczLiczbowy[i] = ascii - 97;
		}
	}
}

void usunListe(Bufor** glowa)
{
	Bufor* obecny = *glowa;
	Bufor* nastepny;

	while (obecny != NULL)
	{
		nastepny = obecny->next;
		free(obecny);
		obecny = nastepny;
	}
	*glowa = NULL;
}