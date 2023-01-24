#include "racional.hpp"

// COST: O(n), ja que utilitza la funció "mcd", que té un cost O(n).
racional::racional(int n, int d) throw(error)
{
   // Si el denominador és zero, es llença l'error "El denominador es zero".
   if (d == 0)
      throw error(DenominadorZero);

   bool negatiu = (n < 0 and d > 0) or (n > 0 and d < 0);
   if (n < 0)
      n = -n;
   if (d < 0)
      d = -d;
   int max_comu_div = mcd(n, d);
   _n = n / max_comu_div;
   _d = d / max_comu_div;
   if (negatiu)
      _n = -_n;
}

// COST: O(1), ja que només copia els valors dels atributs "_n" i "_d" de "r" a la instància actual.
racional::racional(const racional &r) throw(error)
{
   *this = r;
}

// COST: O(1), ja que només copia els valors dels atributs "_n" i "_d" de "r" a la instància actual.
racional &racional::operator=(const racional &r) throw(error)
{
   if (this != &r)
   {
      _n = r._n;
      _d = r._d;
   }
   return *this;
}

// COST: O(1), perquè no fa res més que cridar el destructor de la classe.
racional::~racional() throw() {}

// COST: O(1), ja que només retorna el valor d'un atribut d'instància.
int racional::num() const throw()
{
   return _n;
}

// COST: O(1), ja que només retorna el valor d'un atribut d'instància.
int racional::denom() const throw()
{
   return _d;
}

// COST: O(1), ja que només realitza dos càlculs amb enters.
int racional::part_entera() const throw()
{
   int q = _n / _d;
   int r = _n % _d;
   if (r < 0)
      --q;
   return q;
}

// COST: O(1), ja que utilitza el constructor de còpia, que té un cost O(1), i la funció "part_entera", que té un cost O(1).
racional racional::residu() const throw()
{
   return racional((_n - _d * part_entera()), _d);
}

// COST: O(1), ja que utilitza el constructor de còpia de la classe "racional", que té un cost O(1).
racional racional::operator+(const racional &r) const throw(error)
{
   if (_d == r._d)
      return racional(_n + r._n, _d);
   else
   {
      int num = _d * r._d / _d * _n + _d * r._d / r._d * r._n;
      int denom = _d * r._d;
      return racional(num, denom);
   }
}

// COST: O(1), ja que utilitza el constructor de còpia de la classe "racional", que té un cost O(1).
racional racional::operator-(const racional &r) const throw(error)
{
   if (_d == r._d)
      return racional(_n - r._n, _d);
   else
   {
      int num = _d * r._d / _d * _n - _d * r._d / r._d * r._n;
      int denom = _d * r._d;
      return racional(num, denom);
   }
}

// COST: O(1), ja que utilitza el constructor de còpia de la classe "racional", que té un cost O(1).
racional racional::operator*(const racional &r) const throw(error)
{
   int num = _n * r._n;
   int denom = _d * r._d;
   return racional(num, denom);
}

// COST: O(1), ja que utilitza el constructor de còpia de la classe "racional", que té un cost O(1).
racional racional::operator/(const racional &r) const throw(error)
{
   int num = _n * r._d;
   int denom = _d * r._n;
   return racional(num, denom);
}

// COST: O(1), ja que només realitza dues comparacions amb enters.
bool racional::operator==(const racional &r) const throw()
{
   return _n == r._n and _d == r._d;
}

// COST: O(1), ja que només realitza dues comparacions amb enters.
bool racional::operator!=(const racional &r) const throw()
{
   return not(*this == r);
}

// COST: O(1), ja que només realitza dues multiplicacions d'enters i una comparació amb enters.
bool racional::operator<(const racional &r) const throw()
{
   return _n * r._d < r._n * _d;
}

// COST: O(1), ja que només realitza dues multiplicacions d'enters i una comparació amb enters.
bool racional::operator<=(const racional &r) const throw()
{
   return _n * r._d <= r._n * _d;
}

// COST: O(1), ja que només realitza dues multiplicacions d'enters i una comparació amb enters.
bool racional::operator>(const racional &r) const throw()
{
   return not(*this <= r);
}

// COST: O(1), ja que només realitza dues multiplicacions d'enters i una comparació amb enters.
bool racional::operator>=(const racional &r) const throw()
{
   return not(*this < r);
}

// COST: O(n), ja que utilitza un bucle "while" per calcular el MCD.
int racional::mcd(int a, int b)
{
   while (b != 0)
   {
      int r = a % b;
      a = b;
      b = r;
   }
   return a;
}

// COST: O(n), ja que utilitza la funció "mcd", que té un cost O(n).
int racional::mcm(int a, int b)
{
   return (a * b) / mcd(a, b);
}
