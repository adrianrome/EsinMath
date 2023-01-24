#include "math_sessio.hpp"

// COST: O(log n), ja que la funció "assign" té cost O(log n).
math_sessio::math_sessio() throw(error)
{
    _vars.assign("%", expressio());
    _bybye = false;
}

// COST: O(n), ja que la funció "variables::operator=" té cost O(n).
math_sessio::math_sessio(const math_sessio &es) throw(error)
{
    _vars = es._vars;
    _bybye = es._bybye;
}

// COST: O(n), ja que la funció "variables::operator=" té cost O(n).
math_sessio &math_sessio::operator=(const math_sessio &es) throw(error)
{
    if (this != &es)
    {
        _vars = es._vars;
        _bybye = es._bybye;
    }
    return *this;
}

// COST: O(n), ja que la funció "~variables()" té cost O(n).
math_sessio::~math_sessio() throw(error) {}

// COST: O(n), ja que utilitza un bucle "for" per recórrer la llista de tokens de l'expressió.
void math_sessio::execute(const list<token> &lin, list<token> &lout) throw(error)
{
    // Analitza parcialment lin per verificar si la comanda és correcta.
    analisi_comanda(lin);
    // Final de sessió: byebye.
    if (lin.front().tipus() == token::BYEBYE)
    {
        _bybye = true;
        lout = list<token>();
    }
    // Desassignació d'una variable v: unassign v.
    else if (lin.front().tipus() == token::DESASSIGNACIO)
    {
        _vars.unassign(lin.back().identificador_variable());
        // Retorna la llista que conté com únic token la variable desassignada.
        lout.push_back(lin.back());
    }
    // Assignació v :=  E. S'avalua E i s'assigna el resultat a la variable de nom v.
    else if ((*next(lin.begin(), 1)).tipus() == token::ASSIGNACIO)
    {
        // Creem una llista de tokens amb l'expressió a assignar.
        list<token> lt_exp;
        auto id = lin.begin();
        auto it = id;
        advance(it, 2);
        for (; it != lin.end(); ++it)
        {
            lt_exp.push_back(*it);
        }
        expressio exp(lt_exp);
        // Simplifiquem l'expressió i apliquem totes les substitucions.
        exp.simplify();
        apply_all_substitutions(exp);
        exp.simplify();
        list<string> lt_vars;
        exp.vars(lt_vars);
        for (list<string>::iterator it = lt_vars.begin(); it != lt_vars.end(); ++it)
            if ((*it) == (*id).identificador_variable())
                // Si la variable és una expressió que conté la mateixa variable, es llença l'error "Assignació amb circularitat infinita".
                throw error(AssigAmbCirculInfinita);
        // Assignem el valor de l'expressió simplificada a la variable.
        _vars.assign("%", exp);
        _vars.assign((*id).identificador_variable(), exp);
        exp.list_of_tokens(lout);
    }
    // Avaluació d'una expressió E.
    else
    {
        // Creem una expressió a partir d'una llista de tokens.
        expressio exp(lin);
        // Simplifiquem l'expressió i apliquem totes les substitucions.
        apply_all_substitutions(exp);
        exp.simplify();
        _vars.assign("%", exp);
        exp.list_of_tokens(lout);
    }
}

// COST: O(1), ja que només retorna una variable booleana.
bool math_sessio::end_of_session() const throw()
{
    return _bybye;
}

// COST: O(n), ja que utilitza un bucle "for" per recórrer totes les variables assignades al "math_sessio" actual.
void math_sessio::dump(list<string> &l) const throw(error)
{
    list<string> lk;
    _vars.dump(lk);
    for (list<string>::iterator it = lk.begin(); it != lk.end(); ++it)
    {
        expressio x = _vars.valor(*it);
        list<token> ltv;
        x.list_of_tokens(ltv);
        string frase = *it + " = " + math_io::tostring(ltv);
        ltv.clear();
        l.push_back(frase);
    }
}

// COST: O(n^2) ja que utilitza un bucle "while" dins d'un altre bucle "for" per aplicar les substitucions.
void math_sessio::apply_all_substitutions(expressio &e) const throw(error)
{
    bool te_substitucio = true;
    while (te_substitucio)
    {
        te_substitucio = false;
        list<string> vars;
        e.vars(vars);
        for (list<string>::iterator it = vars.begin(); it != vars.end(); ++it)
            if (not _vars.valor(*it))
            {
                e.apply_substitution(*it, _vars.valor(*it));
                te_substitucio = true;
            }
    }
}

// COST: O(n), perquè utilitza un iterador per recórrer la llista de tokens "lin" i comprova cada token.
void math_sessio::analisi_comanda(const list<token> &lin)
{
    for (list<token>::const_iterator it = lin.begin(); it != lin.end(); ++it)
    {
        // Conté el token DESASSIGNACIO i...
        if (*it == token(token::DESASSIGNACIO))
        {
            // ...la comanda no té dos tokens.
            if (lin.size() != 2)
                throw error(SintComandaIncorrecta);
            // ...aquest no és el primer token.
            else if (lin.front().tipus() != token::DESASSIGNACIO)
                throw error(SintComandaIncorrecta);
            // ...el segon token no és una VARIABLE.
            else if (not es_segon_token(lin, token::VARIABLE))
                throw error(SintComandaIncorrecta);
        }
        // Conté el token BYEBYE...
        else if (*it == token(token::BYEBYE))
        {
            // ...i aquest no és el primer.
            if (lin.front().tipus() != token::BYEBYE)
                throw error(SintComandaIncorrecta);
            // ...no és l'únic token de la comanda.
            else if (lin.size() != 1)
                throw error(SintComandaIncorrecta);
        }
        // Conté el token ASSIGNACIO...
        else if (*it == token(token::ASSIGNACIO))
        {
            // ...la comanda té longitud menor que dos.
            if (lin.size() <= 2)
                throw error(SintComandaIncorrecta);
            // ...no és el segon token.
            else if (not es_segon_token(lin, token::ASSIGNACIO))
                throw error(SintComandaIncorrecta);
            // ...el primer token no és un token VARIABLE.
            else if (lin.front().tipus() != token::VARIABLE)
                throw error(SintComandaIncorrecta);
        }
    }
}

// COST: O(1), ja que només es fa una única iteració sobre una llista.
bool math_sessio::es_segon_token(const list<token> &lin, const token::codi &c)
{
    auto it = std::next(lin.begin());
    return it != lin.end() and (*it).tipus() == c;
}
