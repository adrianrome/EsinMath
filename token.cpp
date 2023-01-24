#include "token.hpp"

// COST: O(1), ja que només realitza una crida a la funció "es_operand" que té un cost O(1).
token::token(codi cod) throw(error)
{
    if (es_operand(cod))
        // Si el codi és d'un operand, es llença l'error "La constructora per aquest token no és l'adequada".
        throw error(ConstructoraInadequada);
    _codi = cod;
}

// COST: O(1), ja que només assigna valors a dos atributs de la instància de "token".
token::token(int n) throw(error)
{
    _codi = CT_ENTERA;
    _valor.i = n;
}

// COST: O(1), ja que només assigna valors a dos atributs de la instància de "token".
token::token(const racional &r) throw(error)
{
    _codi = CT_RACIONAL;
    _valor.r = r;
}

// COST: O(1), ja que només assigna valors a dos atributs de la instància de "token".
token::token(double x) throw(error)
{
    _codi = CT_REAL;
    _valor.d = x;
}

// COST: O(n), on n és la mida de la string "var_name", ja que hi ha un "for" que itera tota la string.
token::token(const string &var_name) throw(error)
{
    if (var_name.empty() or es_paraula_reservada(var_name))
        // Si la string és una paraula reservada, es llença l'error "El nom de la variable no és vàlid".
        throw error(IdentificadorIncorrecte);
    for (int i = 0; i < int(var_name.size()); i++)
        if (not(es_caracter_valid(var_name[i])))
            // Si la string conté un caràcter no adequat, es llença l'error "El nom de la variable no és vàlid".
            throw error(IdentificadorIncorrecte);
    _codi = VARIABLE;
    _valor.s = new string(var_name);
}

// COST: O(1), ja que només assigna valors a dos atributs.
token::token(const token &t) throw(error)
{
    _codi = t._codi;
    switch (_codi)
    {
    case CT_ENTERA:
        _valor.i = t._valor.i;
        break;
    case CT_RACIONAL:
        _valor.r = t._valor.r;
        break;
    case CT_REAL:
        _valor.d = t._valor.d;
        break;
    case VARIABLE:
        delete _valor.s;
        _valor.s = new string(*t._valor.s);
        break;
    default:
        break;
    }
}

// COST: O(1), ja que només assigna valors a dos atributs.
token &token::operator=(const token &t) throw(error)
{
    if (this != &t)
    {
        if (_codi == VARIABLE)
            delete _valor.s;
        _codi = t._codi;
        switch (_codi)
        {
        case CT_ENTERA:
            _valor.i = t._valor.i;
            break;
        case CT_RACIONAL:
            _valor.r = t._valor.r;
            break;
        case CT_REAL:
            _valor.d = t._valor.d;
            break;
        case VARIABLE:
            _valor.s = new string(*t._valor.s);
            break;
        default:
            break;
        }
    }
    return *this;
}

// COST: O(1), perquè no fa res més que cridar el destructor de la classe.
token::~token() throw()
{
    if (_codi == VARIABLE)
        delete _valor.s;
}

// COST: O(1), ja que només retorna el valor d'un atribut.
token::codi token::tipus() const throw()
{
    return _codi;
}

// COST: O(1), ja que només retorna el valor d'un atribut.
int token::valor_enter() const throw(error)
{
    if (_codi == CT_ENTERA)
        return _valor.i;
    else
        // Si la instància no és una constant entera, es llença l'error "Aquesta consultora del token no és apropiada".
        throw error(ConsultoraInadequada);
}

// COST: O(1), ja que només retorna el valor d'un atribut.
racional token::valor_racional() const throw(error)
{
    if (_codi == CT_RACIONAL)
        return _valor.r;
    else
        // Si la instància no és una constant racional, es llença l'error "Aquesta consultora del token no és apropiada".
        throw error(ConsultoraInadequada);
}

// COST: O(1), ja que només retorna el valor d'un atribut.
double token::valor_real() const throw(error)
{
    if (_codi == CT_REAL)
        return _valor.d;
    else
        // Si la instància no és una constant real, es llença l'error "Aquesta consultora del token no és apropiada".
        throw error(ConsultoraInadequada);
}

// COST: O(1), ja que només retorna el valor d'un atribut.
string token::identificador_variable() const throw(error)
{
    if (_codi == VARIABLE)
        return *_valor.s;
    else
        // Si la instància no és una variable, es llença l'error "Aquesta consultora del token no és apropiada".
        throw error(ConsultoraInadequada);
}

// COST: O(1), ja que només compara els valors d'alguns atributs.
bool token::operator==(const token &t) const throw()
{
    if (_codi == t._codi)
    {
        switch (_codi)
        {
        case CT_ENTERA:
            return _valor.i == t._valor.i;
        case CT_RACIONAL:
            return _valor.r == t._valor.r;
        case CT_REAL:
            return _valor.d == t._valor.d;
        case VARIABLE:
            return *_valor.s == *t._valor.s;
        default:
            return true;
        }
    }
    else
        return false;
}

// COST: O(1), ja que només compara els valors d'alguns atributs.
bool token::operator!=(const token &t) const throw()
{
    return not(*this == t);
}

// COST: O(1), ja que només compara els valors dels codis de les instàncies de "token" amb constants predefinides.
bool token::operator>(const token &t) const throw(error)
{
    if (es_operador(_codi) and es_operador(t._codi))
        return obtenir_precedencia(_codi) > obtenir_precedencia(t._codi);
    else
        // Si un dels tokens no és un operador, es llença l'error "La precedència es defineix entre operadors".
        throw error(PrecedenciaEntreNoOperadors);
}

// COST: O(1), ja que només compara els valors dels codis de les instàncies de "token" amb constants predefinides.
bool token::operator<(const token &t) const throw(error)
{
    if (es_operador(_codi) and es_operador(t._codi))
        return obtenir_precedencia(_codi) < obtenir_precedencia(t._codi);
    else
        // Si un dels tokens no és un operador, es llença l'error "La precedència es defineix entre operadors".
        throw error(PrecedenciaEntreNoOperadors);
}

// COST: O(1), ja que només realitza una comprovació amb un "switch-case".
bool token::es_operand(const codi &c)
{
    switch (c)
    {
    case CT_ENTERA:
    case CT_RACIONAL:
    case CT_REAL:
    case VARIABLE:
        return true;
    default:
        return false;
    }
}

// COST: O(1), ja que només realitza una comprovació amb un "switch-case".
bool token::es_operador(const codi &c)
{
    switch (c)
    {
    case SUMA:
    case RESTA:
    case MULTIPLICACIO:
    case DIVISIO:
    case CANVI_DE_SIGNE:
    case SIGNE_POSITIU:
    case EXPONENCIACIO:
        return true;
    default:
        return false;
    }
}

// COST: O(1), ja que només realitza una comprovació amb un "switch-case".
int token::obtenir_precedencia(const codi &c)
{
    switch (c)
    {
    case SUMA:
    case RESTA:
        return 1;
    case MULTIPLICACIO:
    case DIVISIO:
        return 2;
    case CANVI_DE_SIGNE:
    case SIGNE_POSITIU:
        return 3;
    case EXPONENCIACIO:
        return 4;
    default:
        return 0;
    }
}

// COST: O(1), ja que només realitza comparacions bàsiques.
bool token::es_caracter_valid(const char &c)
{
    return (c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z') or c == '_';
}

// COST: O(1), ja que només realitza comparacions bàsiques.
bool token::es_paraula_reservada(const string &s)
{
    return s == "unassign" or s == "e" or s == "sqrt" or s == "log" or s == "exp" or s == "evalf";
}
