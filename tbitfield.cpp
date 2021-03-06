

#include "tbitfield.h"

TBitField::TBitField(int len)
{
	if (len < 0)
		throw std::logic_error("Input error: invalide value of bitfield length in constructor");
	else
	{
		BitLen = len;
		MemLen = len / (sizeof(TELEM) * 8) + 1;
		pMem = new TELEM[MemLen];
		for (int i = 0; i < MemLen; i++)
			pMem[i] = 0;
	}
}

TBitField::TBitField(const TBitField& bf) // конструктор копирования
{
	BitLen = bf.BitLen;
	MemLen = bf.MemLen;
	pMem = new TELEM[MemLen];
	for (int i = 0; i < MemLen; i++)
	{
		pMem[i] = bf.pMem[i];
	}
}

TBitField::~TBitField()
{
	delete[]pMem;
	pMem = NULL;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
	if (n < 0)
		throw std::out_of_range("Input error: invalide value of bitfield in GetMemIndex");
	else
		return(n / (sizeof(TELEM) * 8));
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
	if (n < 0)
		throw std::out_of_range("Input error: invalide value of bitfield in GetMemMask");
	else
	{
		int tmp = n % (sizeof(int) * 8);
		TELEM mask = 1 << tmp;
		return mask;
	}
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
	return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
	if (n < 0 || n > BitLen)
	{
		throw std::out_of_range("Input error: invalide value of bitfield in SetBit");
	}
	pMem[GetMemIndex(n)] |= GetMemMask(n);
}

void TBitField::ClrBit(const int n) // очистить бит
{
	if (n < 0 || n > BitLen)
	{
		throw std::out_of_range("Input error: invalide value of bitfield in ClrBit");
	}
	pMem[GetMemIndex(n)] &= ~GetMemMask(n);
}

int TBitField::GetBit(const int n) const // получить значение бита
{
	if (n < 0 || n > BitLen)
	{
		throw std::out_of_range("Input error: invalide value of bitfield in GetBit");
	}
	return pMem[GetMemIndex(n)] & GetMemMask(n);
}
// битовые операции

TBitField& TBitField::operator=(const TBitField& bf) // присваивание
{
	if (!(*this == bf))
	{
		if (MemLen != bf.MemLen)
		{
			delete[] pMem;
			MemLen = bf.MemLen;
			pMem = new TELEM[MemLen];
		}
		BitLen = bf.BitLen;
		for (int i = 0; i < MemLen; i++)
		{
			pMem[i] = bf.pMem[i];
		}
		return *this;
	}
	else return *this;
}

bool TBitField::operator==(const TBitField& bf) const // сравнение
{
	if (BitLen != bf.BitLen)
	{
		return false;
	}
	else
	{
		for (int i = 0; i < MemLen; i++)
		{
			if (pMem[i] != bf.pMem[i])
			{
				return false;
			}
			return true;
		}
	}
}

bool TBitField::operator!=(const TBitField& bf) const // сравнение
{
	if (!(*this == bf))
		return true;
	else return false;
}

TBitField TBitField::operator|(const TBitField& bf) // операция "или"
{
	int len = BitLen;
	if (bf.BitLen > len)
		len = bf.BitLen;
	TBitField temp(len);
	for (int i = 0; i < MemLen; i++)
	{
		temp.pMem[i] = pMem[i];
	}
	for (int i = 0; i < bf.MemLen; i++)
	{
		temp.pMem[i] |= bf.pMem[i];
	}

	return temp;
}

TBitField TBitField::operator&(const TBitField& bf) // операция "и"
{
	TBitField tmp(bf.BitLen > BitLen ? bf.BitLen : BitLen);

	for (int i = 0; i < MemLen; i++)
	{
		tmp.pMem[i] = pMem[i];
	}
	for (int i = 0; i < bf.MemLen; i++)
	{
		tmp.pMem[i] &= bf.pMem[i];
	}

	return tmp;
}

TBitField TBitField::operator~(void) // отрицание
{
	TBitField tmp(*this);

	for (int i = 0; i < tmp.BitLen; i++)
	{
		if (tmp.GetBit(i))
		{
			tmp.ClrBit(i);
		}
		else
		{
			tmp.SetBit(i);
		}
	}
	return tmp;
}

void TBitField::InFile(std::string file_name)
{
	fstream fs;
	fs.open(file_name, fstream::in | fstream::out);
	if (!fs.is_open())
	{
		std::cout << "Error open file!" << std::endl;
	}
	else
	{	
		for (int i = 0; i < GetLength(); i++)
		{
			if (GetBit(i))
			{
				fs << "1";
			}
			else
			{
				fs << "0";
			}
		}
		std::cout << "File opened and rewritten!" << std::endl;
	}
	fs.close();
}

void TBitField::FromFile(std::string file_name)
{
	fstream fs;
	fs.open(file_name, fstream::in | fstream::out);
	if (!fs.is_open())
	{
		std::cout << "Error open file!" << std::endl;
	}
	else
	{
		std::string str; 
		fs >> str;
		TBitField tmp(str.size());
		for (int i = 0; i < str.size(); i++)
		{
			if (str[i] == '1')
			{
				tmp.SetBit(i);
			}
			else
			{
				tmp.ClrBit(i);
			}
		}
		*this = tmp;
		std::cout << "File is open and read!" << std::endl;
	}
	fs.close();
}

// ввод/вывод
istream& operator>>(istream& istr, TBitField& bf) // ввод
{
	char sym;
	do {
		istr >> sym;
	} while (sym != ' ');
	int i = 0;
	while (true)
	{
		istr >> sym;
		if (sym == '0')
			bf.ClrBit(i++);
		else if (sym == '1')
			bf.SetBit(i++);
		else break;
	}
	return istr;
}

ostream& operator<<(ostream& ostr, const TBitField& bf) // вывод
{
	int len = bf.GetLength();
	for (int i = 0; i < len; i++)
		if (bf.GetBit(i)) ostr << '1';
		else ostr << '0';
	return ostr;
}