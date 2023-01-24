#include <cmath>
#include "expressio.hpp"

expressio::expressio(const token t) throw(error)
{
    if (not(t.tipus() >= 0 and t.tipus() <= 6))
        // Si hi ha un error sintàctic, es llença l'error "Error sintàctic".
        throw error(ErrorSintactic);
    _arrel = new node(t);
}

expressio::expressio(const list<token> &l) throw(error)
{
    list<node *> expressio_fragments;
    list<token> pila_operadors;
    int n_par_oberts = 0;
    for (list<token>::const_iterator it = l.begin(); it != l.end(); ++it)
        // Si és un "(", l’apilem a la pila d’operadors.
        if (it->tipus() == token::OBRIR_PAR)
        {
            pila_operadors.push_back(*it);
            ++n_par_oberts;
        }
        // Si és un ")", desapilarem els operadors de la pila d’operadors fins que trobem un "(".
        else if (it->tipus() == token::TANCAR_PAR)
        {
            if (n_par_oberts == 0)
            {
                while (not expressio_fragments.empty())
                {
                    delete expressio_fragments.back();
                    expressio_fragments.pop_back();
                }
                // Si hi ha un parèntesi tancat ")" i el nombre de parèntesis oberts és 0, es llença l'error "Error sintàctic".
                throw error(ErrorSintactic);
            }
            // Mentre no trobem un parèntesi obert "(", desapilarem els operadors de la pila d’operadors fins que trobem un "(".
            while (pila_operadors.back().tipus() != token::OBRIR_PAR)
            {
                token top = pila_operadors.back();
                pila_operadors.pop_back();
                node *operador = new node(top);
                node *fragment = expressio_fragments.back();
                expressio_fragments.pop_back();
                operador->f_dret = fragment;
                // Si l'operador és un operador binari, s'utilitzen dos operands per crear l'expressió.
                if (es_operador_binari(top))
                {
                    node *fragment = expressio_fragments.back();
                    expressio_fragments.pop_back();
                    operador->f_esq = fragment;
                }
                // Apilem l’expressió creada a la pila d’expressions.
                expressio_fragments.push_back(operador);
            }
            pila_operadors.pop_back();
            // Si és una funció, es desapilen els operadors de la pila d'operadors deixant el fill esquerre buit.
            if (es_funcio(pila_operadors.back()))
            {
                token top = pila_operadors.back();
                pila_operadors.pop_back();
                node *operador = new node(top);
                node *fragment = expressio_fragments.back();
                expressio_fragments.pop_back();
                operador->f_dret = fragment;
                expressio_fragments.push_back(operador);
            }
            --n_par_oberts;
        }
        // Si és un operand, crearem una expressió que només conté l’operand i l’apilarem a la pila d’expressions.
        else if (es_operand(*it))
        {
            node *operand = new node(*it);
            expressio_fragments.push_back(operand);
            list<token>::const_iterator next = it;
            ++next;
            if (next != l.end() and es_operand(*next))
            {
                while (not expressio_fragments.empty())
                {
                    delete expressio_fragments.back();
                    expressio_fragments.pop_back();
                }
                // Si el següent token també és un operand, es llença l'error "Error sintàctic".
                throw error(ErrorSintactic);
            }
        }
        // Si es troba un símbol de funció, s'apila a la pila d'operadors.
        else if (es_funcio(*it))
        {
            list<token>::const_iterator next = it;
            ++next;
            if (next != l.end() and next->tipus() != token::OBRIR_PAR)
            {
                while (not expressio_fragments.empty())
                {
                    delete expressio_fragments.back();
                    expressio_fragments.pop_back();
                }
                // Si després d'un símbol de funció no hi ha un parèntesi obert "(", es llença l'error "Error sintàctic".
                throw error(ErrorSintactic);
            }
            pila_operadors.push_back(*it);
        }
        // Si es troba un operador unari, s'apila a la pila d'operadors.
        else if (es_operador_unari(*it))
            pila_operadors.push_back(*it);
        // Si es troba un operador binari...
        else if (es_operador_binari(*it))
        {
            // ...mentre l’operador té prioritat inferior (o igual i té l’associativitat d’esquerra a dreta) que el capdamunt de la pila d’operadors, desapilarem l’operador de la pila i crearem una expressió.
            while (not pila_operadors.empty() and pila_operadors.back().tipus() != token::OBRIR_PAR and (*it < pila_operadors.back() or (prioritat_igual(*it, pila_operadors.back()) and associativitat_ed(*it))))
            {
                token top = pila_operadors.back();
                pila_operadors.pop_back();
                node *operador = new node(top);
                node *fragment = expressio_fragments.back();
                expressio_fragments.pop_back();
                operador->f_dret = fragment;
                // Si l'operador es un operador binari, s'utilitzen dos operands per crear l'expressió.
                if (es_operador_binari(top))
                {
                    node *fragment = expressio_fragments.back();
                    expressio_fragments.pop_back();
                    operador->f_esq = fragment;
                }
                expressio_fragments.push_back(operador);
            }
            // Sempre al final hem d’apilar l’operador d’entrada a la pila d’operadors.
            pila_operadors.push_back(*it);
        }
        else
        {
            while (not expressio_fragments.empty())
            {
                delete expressio_fragments.back();
                expressio_fragments.pop_back();
            }
            throw error(ErrorSintactic);
        }
    if (n_par_oberts != 0)
    {
        while (not expressio_fragments.empty())
        {
            delete expressio_fragments.back();
            expressio_fragments.pop_back();
        }
        // Si n_par_oberts és diferent de 0, es llença l'error "Error sintàctic".
        throw error(ErrorSintactic);
    }
    // Un cop es fa la lectura de tota l'expressió, es desapilen els operadors que quedin a la pila d'operadors i es creen les expressions corresponents.
    while (not pila_operadors.empty())
    {
        token top = pila_operadors.back();
        pila_operadors.pop_back();
        node *operador = new node(top);
        node *fragment = expressio_fragments.back();
        expressio_fragments.pop_back();
        operador->f_dret = fragment;
        // Si l'operador és un operador binari, s'utilitzen dos operands per crear l'expressió.
        if (es_operador_binari(top))
        {
            node *fragment = expressio_fragments.back();
            expressio_fragments.pop_back();
            operador->f_esq = fragment;
        }
        // Sempre al final hem d’apilar l’operador d’entrada a la pila d’operadors.
        expressio_fragments.push_back(operador);
    }
    if (expressio_fragments.empty())
    {
        while (not expressio_fragments.empty())
        {
            delete expressio_fragments.back();
            expressio_fragments.pop_back();
        }
        // Si a la final de tot no hi ha una expressió a la pila d'expressions, es llença l'error "Error sintàctic".
        throw error(ErrorSintactic);
    }
    // Es guarda l'única expressió restant a la pila d'expressions com a l'expressió final.
    _arrel = expressio_fragments.back();
    expressio_fragments.pop_back();
}

expressio::expressio(const expressio &e) throw(error)
{
    _arrel = copiar_arbre(e._arrel);
}

expressio &expressio::operator=(const expressio &e) throw(error)
{
    if (this != &e)
    {
        esborra_arbre(_arrel);
        _arrel = copiar_arbre(e._arrel);
    }
    return *this;
}

expressio::~expressio() throw(error)
{
    esborra_arbre(_arrel);
    _arrel = nullptr;
}

expressio::operator bool() const throw()
{
    return _arrel == nullptr or _arrel->info.tipus() == token::NULLTOK;
}

bool expressio::operator==(const expressio &e) const throw()
{
    return arbres_iguals(_arrel, e._arrel);
}

bool expressio::operator!=(const expressio &e) const throw()
{
    return not(*this == e);
}

void expressio::vars(list<string> &l) const throw(error)
{
    troba_vars(_arrel, l);
}

void expressio::apply_substitution(const string &v, const expressio &e) throw(error)
{
    variable_substitutiva(_arrel, v, e);
}

void expressio::simplify_one_step() throw(error)
{
    bool simplificat = false;
    simplifica_parcial(_arrel, simplificat);
}

void expressio::simplify() throw(error)
{
    bool hi_ha_canvis = true;
    while (hi_ha_canvis)
    {
        hi_ha_canvis = false;
        simplifica_total(_arrel, hi_ha_canvis);
    }
}

void expressio::list_of_tokens(list<token> &lt) throw(error)
{
    list<token> pila;
    recorrer_arbre(_arrel, lt, pila);
}

bool expressio::es_constant(const token &t)
{
    switch (t.tipus())
    {
    case token::CT_ENTERA:
    case token::CT_RACIONAL:
    case token::CT_REAL:
    case token::CT_E:
        return true;
    default:
        return false;
    }
}

bool expressio::es_operand(const token &t)
{
    return es_constant(t) or t.tipus() == token::VARIABLE or t.tipus() == token::VAR_PERCENTATGE;
}

bool expressio::es_operador_binari(const token &t)
{
    switch (t.tipus())
    {
    case token::SUMA:
    case token::RESTA:
    case token::MULTIPLICACIO:
    case token::DIVISIO:
    case token::EXPONENCIACIO:
        return true;
    default:
        return false;
    }
}

bool expressio::es_operador_unari(const token &t)
{
    switch (t.tipus())
    {
    case token::CANVI_DE_SIGNE:
    case token::SIGNE_POSITIU:
        return true;
    default:
        return false;
    }
}

bool expressio::es_operador(const token &t)
{
    return es_operador_binari(t) or es_operador_unari(t);
}

bool expressio::es_funcio(const token &t)
{
    switch (t.tipus())
    {
    case token::SQRT:
    case token::EXP:
    case token::LOG:
    case token::EVALF:
        return true;
    default:
        return false;
    }
}

int expressio::obtenir_precedencia(const token &t)
{
    switch (t.tipus())
    {
    case token::SUMA:
    case token::RESTA:
        return 1;
    case token::MULTIPLICACIO:
    case token::DIVISIO:
        return 2;
    case token::CANVI_DE_SIGNE:
    case token::SIGNE_POSITIU:
        return 3;
    case token::EXPONENCIACIO:
        return 4;
    default:
        return -1;
    }
}

bool expressio::prioritat_igual(const token &t1, const token &t2)
{
    return obtenir_precedencia(t1) == obtenir_precedencia(t2);
}

bool expressio::associativitat_ed(const token &t)
{
    switch (t.tipus())
    {
    case token::SUMA:
    case token::RESTA:
    case token::MULTIPLICACIO:
    case token::DIVISIO:
    case token::CANVI_DE_SIGNE:
    case token::SIGNE_POSITIU:
        return true;
    case token::EXPONENCIACIO:
    default:
        return false;
    }
}

void expressio::recorrer_arbre(node *n, list<token> &lt, list<token> &p)
{
    if (n != nullptr)
    {
        bool exponen = false;
        if (not(p.empty()) and es_operador(n->info) and es_operador(p.back()) and not(n->info.tipus() == token::CANVI_DE_SIGNE))
            if ((obtenir_precedencia(n->info) < obtenir_precedencia(p.back())) and not(n->info.tipus() == token::EXPONENCIACIO))
                lt.push_back(token(token::OBRIR_PAR));
        if (p.back().tipus() == token::EXPONENCIACIO)
            exponen = true;
        if (es_operador(n->info) or es_funcio(n->info))
            p.push_back(n->info);
        if (n->info.tipus() == token::EXPONENCIACIO and (n->f_esq->info.tipus() == token::EXPONENCIACIO))
            lt.push_back(token(token::OBRIR_PAR));
        if (n->info.tipus() == token::EXPONENCIACIO and n->f_esq->info.tipus() == token::CT_RACIONAL and exponen)
            lt.push_back(token(token::OBRIR_PAR));
        // Se'n va a l'esquerra
        recorrer_arbre(n->f_esq, lt, p);
        if (n->info.tipus() == token::EXPONENCIACIO and n->f_esq->info.tipus() == token::CT_RACIONAL and exponen)
            lt.push_back(token(token::TANCAR_PAR));
        if (n->info.tipus() == token::EXPONENCIACIO and n->f_esq->info.tipus() == token::EXPONENCIACIO)
            lt.push_back(token(token::TANCAR_PAR));
        // Es posa el token a la llista
        lt.push_back(n->info);
        if (es_funcio(n->info) or ((n->info.tipus() == token::DIVISIO or n->info.tipus() == token::RESTA) and (obtenir_precedencia(n->info) == obtenir_precedencia(n->f_dret->info))))
            lt.push_back(token(token::OBRIR_PAR));
        if (n->info.tipus() == token::EXPONENCIACIO and n->f_dret->info.tipus() == token::CT_RACIONAL)
            lt.push_back(token(token::OBRIR_PAR));
        // Se'n va a la dreta
        recorrer_arbre(n->f_dret, lt, p);
        if (n->info.tipus() == token::EXPONENCIACIO and n->f_dret->info.tipus() == token::CT_RACIONAL)
            lt.push_back(token(token::TANCAR_PAR));
        if (es_funcio(n->info) or ((n->info.tipus() == token::DIVISIO or n->info.tipus() == token::RESTA) and (obtenir_precedencia(n->info) == obtenir_precedencia(n->f_dret->info))))
            lt.push_back(token(token::TANCAR_PAR));
        if (es_operador(n->info) or es_funcio(n->info))
            p.pop_back();
        if (not(p.empty()) and es_operador(n->info) and es_operador(p.back()) and not(n->info.tipus() == token::CANVI_DE_SIGNE))
            if ((obtenir_precedencia(n->info) < obtenir_precedencia(p.back())) and not(n->info.tipus() == token::EXPONENCIACIO))
                lt.push_back(token(token::TANCAR_PAR));
    }
}

expressio::node *expressio::copiar_arbre(node *n)
{
    if (n != nullptr)
    {
        node *nou = new node(n->info);
        nou->f_esq = copiar_arbre(n->f_esq);
        nou->f_dret = copiar_arbre(n->f_dret);
        return nou;
    }
    return nullptr;
}

void expressio::esborra_arbre(node *n)
{
    if (n != nullptr)
    {
        esborra_arbre(n->f_esq);
        esborra_arbre(n->f_dret);
        delete n;
    }
}

bool expressio::arbres_iguals(node *n1, node *n2)
{
    if (n1 == n2)
        return true;
    if (n1 == nullptr or n2 == nullptr)
        return false;
    if (n1->info != n2->info)
        return false;
    return arbres_iguals(n1->f_esq, n2->f_esq) and arbres_iguals(n1->f_dret, n2->f_dret);
}

void expressio::troba_vars(node *n, list<string> &l)
{
    if (n != nullptr)
    {
        if (n->info.tipus() == token::VARIABLE or n->info.tipus() == token::VAR_PERCENTATGE)
        {
            string nom_var;
            if (n->info.tipus() == token::VARIABLE)
                nom_var = n->info.identificador_variable();
            else
                nom_var = "%";
            bool trobat = false;
            for (list<string>::iterator it = l.begin(); it != l.end() and not trobat; ++it)
                if (*it == nom_var)
                    trobat = true;
            if (not trobat)
                l.push_back(nom_var);
        }
        troba_vars(n->f_esq, l);
        troba_vars(n->f_dret, l);
    }
}

void expressio::variable_substitutiva(node *&n, const string &v, const expressio &e)
{
    if (n != nullptr)
    {
        if ((n->info.tipus() == token::VARIABLE and n->info.identificador_variable() == v) or n->info.tipus() == token::VAR_PERCENTATGE)
        {
            node *nou = copiar_arbre(e._arrel);
            esborra_arbre(n);
            n = nou;
        }
        else
        {
            variable_substitutiva(n->f_esq, v, e);
            variable_substitutiva(n->f_dret, v, e);
        }
    }
}

bool expressio::es_negatiu(const token &t)
{
    switch (t.tipus())
    {
    case token::CT_ENTERA:
        return t.valor_enter() < 0;
    case token::CT_RACIONAL:
        return t.valor_racional() < racional(0);
    case token::CT_REAL:
        return t.valor_real() < 0.0;
    default:
        return false;
    }
}

void expressio::simplifica_parcial(node *n, bool &simplificat)
{
    if (n != nullptr)
    {
        simplifica_parcial(n->f_esq, simplificat);
        simplifica_parcial(n->f_dret, simplificat);
        if (not simplificat)
        {
            if (simplifica_racional(n))
                simplificat = true;
            else if (calcula(n))
                simplificat = true;
            else if (simplifica_suma(n))
                simplificat = true;
            else if (simplifica_resta(n))
                simplificat = true;
            else if (simplifica_canvi_de_signe(n))
                simplificat = true;
            else if (simplifica_multiplicacio(n))
                simplificat = true;
            else if (simplifica_divisio(n))
                simplificat = true;
            else if (simplifica_exponenciacio(n))
                simplificat = true;
            else if (simplifica_radicacio(n))
                simplificat = true;
            else if (simplifica_exponencial_i_logaritme(n))
                simplificat = true;
            else if (simplifica_evalf(n))
                simplificat = true;
        }
    }
}

void expressio::simplifica_total(node *n, bool &hi_ha_canvis)
{
    if (n != nullptr)
    {
        simplifica_parcial(n->f_esq, hi_ha_canvis);
        simplifica_parcial(n->f_dret, hi_ha_canvis);
        simplifica_parcial(n, hi_ha_canvis);
    }
}

bool expressio::es_calcul_exponenciacio(node *n)
{
    if (es_constant(n->f_esq->info))
    {
        if (n->f_dret->info.tipus() == token::CT_ENTERA)
        {
            if (n->f_dret->info.valor_enter() != 0 or n->f_dret->info.valor_enter() != 1)
                return true;
        }
        else if (n->f_esq->info.tipus() == token::CT_REAL and n->f_dret->info.tipus() == token::CT_REAL)
            return true;
    }
    return false;
}

bool expressio::simplifica_racional(node *n)
{
    // Tota constant racional c = n/d el denominador d de la qual val 1 (p.e. -2/1) es simplifica a la corresponent constant entera c′ = n.
    if (n->info.tipus() == token::CT_RACIONAL)
        if (n->info.valor_racional().denom() == 1)
        {
            n->info = token(n->info.valor_racional().num());
            return true;
        }
    return false;
}

void expressio::enter_i_racional(node *n)
{
    if (n->f_esq != nullptr and n->f_dret != nullptr)
        if ((n->f_esq->info.tipus() == token::CT_ENTERA and n->f_dret->info.tipus() == token::CT_RACIONAL) or (n->f_esq->info.tipus() == token::CT_RACIONAL and n->f_dret->info.tipus() == token::CT_ENTERA))
        {
            if (n->f_esq->info.tipus() == token::CT_ENTERA)
                n->f_esq->info = token(racional(n->f_esq->info.valor_enter()));
            else
                n->f_dret->info = token(racional(n->f_dret->info.valor_enter()));
        }
}

bool expressio::calcula(node *n)
{
    // Per qualsevol operació binària op ∈ {+, −, ∗, /} i qualssevol constants c1 i c2 del mateix tipus, c1 op c2.
    if (n->info.tipus() == token::SUMA || n->info.tipus() == token::RESTA || n->info.tipus() == token::MULTIPLICACIO || n->info.tipus() == token::DIVISIO)
    {
        enter_i_racional(n);
        if (n->f_esq->info.tipus() == n->f_dret->info.tipus())
        {
            if (n->f_esq->info.tipus() == token::CT_ENTERA)
            {
                int int_resultat;
                racional rac_resultat;
                if (n->info.tipus() == token::SUMA)
                {
                    int_resultat = n->f_esq->info.valor_enter() + n->f_dret->info.valor_enter();
                    n->info = token(int_resultat);
                }
                else if (n->info.tipus() == token::RESTA)
                {
                    int_resultat = n->f_esq->info.valor_enter() - n->f_dret->info.valor_enter();
                    n->info = token(int_resultat);
                }
                else if (n->info.tipus() == token::MULTIPLICACIO)
                {
                    int_resultat = n->f_esq->info.valor_enter() * n->f_dret->info.valor_enter();
                    n->info = token(int_resultat);
                }
                else if (n->info.tipus() == token::DIVISIO)
                {
                    if (n->f_dret->info.valor_enter() != 0)
                        if (n->f_esq->info.valor_enter() % n->f_dret->info.valor_enter() != 0)
                        {
                            rac_resultat = racional(n->f_esq->info.valor_enter(), n->f_dret->info.valor_enter());
                            n->info = token(rac_resultat);
                        }
                        else
                        {
                            int_resultat = n->f_esq->info.valor_enter() / n->f_dret->info.valor_enter();
                            n->info = token(int_resultat);
                        }
                    else
                        // Si c2 és una constant negativa, es llença l'error "Negatiu elevat a no enter".
                        throw error(DivPerZero);
                }
                esborra_arbre(n->f_esq);
                esborra_arbre(n->f_dret);
                n->f_esq = nullptr;
                n->f_dret = nullptr;
                return true;
            }
            else if (n->f_esq->info.tipus() == token::CT_RACIONAL)
            {
                racional resultat;
                if (n->info.tipus() == token::SUMA)
                    resultat = n->f_esq->info.valor_racional() + n->f_dret->info.valor_racional();
                else if (n->info.tipus() == token::RESTA)
                    resultat = n->f_esq->info.valor_racional() - n->f_dret->info.valor_racional();
                else if (n->info.tipus() == token::MULTIPLICACIO)
                    resultat = n->f_esq->info.valor_racional() * n->f_dret->info.valor_racional();
                else if (n->info.tipus() == token::DIVISIO)
                {
                    if (n->f_dret->info.valor_racional() != racional((0)))
                        resultat = n->f_esq->info.valor_racional() / n->f_dret->info.valor_racional();
                    else
                        // Si c2 és una constant negativa, es llença l'error "Negatiu elevat a no enter".
                        throw error(DivPerZero);
                }
                n->info = token(resultat);
                esborra_arbre(n->f_esq);
                esborra_arbre(n->f_dret);
                n->f_esq = nullptr;
                n->f_dret = nullptr;
                return true;
            }
            else if (n->f_esq->info.tipus() == token::CT_REAL)
            {
                double resultat;
                if (n->info.tipus() == token::SUMA)
                    resultat = n->f_esq->info.valor_real() + n->f_dret->info.valor_real();
                else if (n->info.tipus() == token::RESTA)
                    resultat = n->f_esq->info.valor_real() - n->f_dret->info.valor_real();
                else if (n->info.tipus() == token::MULTIPLICACIO)
                    resultat = n->f_esq->info.valor_real() * n->f_dret->info.valor_real();
                else if (n->info.tipus() == token::DIVISIO)
                {
                    if (n->f_dret->info.valor_real() != 0.0)
                        resultat = n->f_esq->info.valor_real() / n->f_dret->info.valor_real();
                    else
                        // Si c2 és una constant negativa, es llença l'error "Negatiu elevat a no enter".
                        throw error(DivPerZero);
                }
                n->info = token(resultat);
                esborra_arbre(n->f_esq);
                esborra_arbre(n->f_dret);
                n->f_esq = nullptr;
                n->f_dret = nullptr;
                return true;
            }
        }
    }
    return false;
}

bool expressio::simplifica_suma(node *n)
{
    // Regles de suma
    if (n->info.tipus() == token::SUMA)
    {
        //(a) 0 + E → E
        if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_esq->info.valor_enter() == 0)
        {
            n->info = n->f_dret->info;
            esborra_arbre(n->f_esq);
            node *aux = n->f_dret;
            n->f_esq = n->f_dret->f_esq;
            n->f_dret = n->f_dret->f_dret;
            delete aux;
            return true;
        }
        //(b) E + 0 → E
        else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
        {
            n->info = n->f_esq->info;
            esborra_arbre(n->f_dret);
            node *aux = n->f_esq;
            n->f_dret = n->f_esq->f_dret;
            n->f_esq = n->f_esq->f_esq;
            delete aux;
            return true;
        }
        //(c) E + E → 2 ∗ E
        else if (arbres_iguals(n->f_esq, n->f_dret))
        {
            n->info = token(token::MULTIPLICACIO);
            esborra_arbre(n->f_esq);
            n->f_esq = new node(token(2));
            return true;
        }
        //(d) E + (−E′) → E − E′
        else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
        {
            n->info = token(token::RESTA);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(e) (−E′) + E → E − E′
        else if (n->f_esq->info.tipus() == token::CANVI_DE_SIGNE)
        {
            n->info = token(token::RESTA);
            node *aux = n->f_esq->f_dret;
            delete n->f_esq;
            n->f_esq = aux;
            return true;
        }
        //(f) E1 ∗ E + E2 ∗ E → (E1 + E2) ∗ E
        else if (n->f_esq->info.tipus() == token::MULTIPLICACIO and n->f_dret->info.tipus() == token::MULTIPLICACIO and arbres_iguals(n->f_esq->f_dret, n->f_dret->f_dret))
        {
            n->f_esq->info = token(token::SUMA);
            esborra_arbre(n->f_esq->f_dret);
            n->f_esq->f_dret = n->f_dret->f_esq;
            n->info = token(token::MULTIPLICACIO);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(g) E1 ∗ E + E ∗ E2 → (E1 + E2) ∗ E
        else if (n->f_esq->info.tipus() == token::MULTIPLICACIO and n->f_dret->info.tipus() == token::MULTIPLICACIO and arbres_iguals(n->f_esq->f_dret, n->f_dret->f_esq))
        {
            n->f_esq->info = token(token::SUMA);
            esborra_arbre(n->f_esq->f_dret);
            n->f_esq->f_dret = n->f_dret->f_dret;
            n->info = token(token::MULTIPLICACIO);
            node *aux = n->f_dret->f_esq;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(h) E ∗ E1 + E2 ∗ E → (E1 + E2) ∗ E
        else if (n->f_esq->info.tipus() == token::MULTIPLICACIO and n->f_dret->info.tipus() == token::MULTIPLICACIO and arbres_iguals(n->f_esq->f_esq, n->f_dret->f_dret))
        {
            n->f_esq->info = token(token::SUMA);
            esborra_arbre(n->f_esq->f_esq);
            n->f_esq->f_esq = n->f_esq->f_dret;
            n->f_esq->f_dret = n->f_dret->f_esq;
            n->info = token(token::MULTIPLICACIO);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(i) E ∗ E1 + E ∗ E2 → (E1 + E2) ∗ E
        else if (n->f_esq->info.tipus() == token::MULTIPLICACIO and n->f_dret->info.tipus() == token::MULTIPLICACIO and arbres_iguals(n->f_esq->f_esq, n->f_dret->f_esq))
        {
            n->f_esq->info = token(token::SUMA);
            esborra_arbre(n->f_esq->f_esq);
            n->f_esq->f_esq = n->f_esq->f_dret;
            n->f_esq->f_dret = n->f_dret->f_dret;
            n->info = token(token::MULTIPLICACIO);
            node *aux = n->f_dret->f_esq;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(j) E1/E + E2/E → (E1 + E2)/E
        else if (n->f_esq->info.tipus() == token::DIVISIO and n->f_dret->info.tipus() == token::DIVISIO and arbres_iguals(n->f_esq->f_dret, n->f_dret->f_dret))
        {
            n->f_esq->info = token(token::SUMA);
            esborra_arbre(n->f_esq->f_dret);
            n->f_esq->f_dret = n->f_dret->f_esq;
            n->info = token(token::DIVISIO);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
    }
    return false;
}

bool expressio::simplifica_resta(node *n)
{
    // Regles de resta
    if (n->info.tipus() == token::RESTA)
    {
        //(a) 0 − E → −E (canvi de signe)
        if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_esq->info.valor_enter() == 0)
        {
            n->info = token(token::CANVI_DE_SIGNE);
            esborra_arbre(n->f_esq);
            n->f_esq = nullptr;
            return true;
        }
        //(b) E − 0 → E
        else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
        {
            n->info = n->f_esq->info;
            esborra_arbre(n->f_dret);
            node *aux = n->f_esq;
            n->f_dret = n->f_esq->f_dret;
            n->f_esq = n->f_esq->f_esq;
            delete aux;
            return true;
        }
        //(c) E − E → 0
        else if (arbres_iguals(n->f_esq, n->f_dret))
        {
            n->info = token(0);
            esborra_arbre(n->f_esq);
            esborra_arbre(n->f_dret);
            n->f_esq = nullptr;
            n->f_dret = nullptr;
            return true;
        }
        //(d) E − (−E′) → E + E
        else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
        {
            n->info = token(token::SUMA);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(e) E1 ∗ E − E2 ∗ E → (E1 − E2) ∗ E
        else if (n->f_esq->info.tipus() == token::MULTIPLICACIO and n->f_dret->info.tipus() == token::MULTIPLICACIO and arbres_iguals(n->f_esq->f_dret, n->f_dret->f_dret))
        {
            n->f_esq->info = token(token::RESTA);
            esborra_arbre(n->f_esq->f_dret);
            n->f_esq->f_dret = n->f_dret->f_esq;
            n->info = token(token::MULTIPLICACIO);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(f) E1 ∗ E − E ∗ E2 → (E1 − E2) ∗ E
        else if (n->f_esq->info.tipus() == token::MULTIPLICACIO and n->f_dret->info.tipus() == token::MULTIPLICACIO and arbres_iguals(n->f_esq->f_dret, n->f_dret->f_esq))
        {
            n->f_esq->info = token(token::RESTA);
            esborra_arbre(n->f_esq->f_dret);
            n->f_esq->f_dret = n->f_dret->f_dret;
            n->info = token(token::MULTIPLICACIO);
            node *aux = n->f_dret->f_esq;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(g) E ∗ E1 − E2 ∗ E → (E1 − E2) ∗ E
        else if (n->f_esq->info.tipus() == token::MULTIPLICACIO and n->f_dret->info.tipus() == token::MULTIPLICACIO and arbres_iguals(n->f_esq->f_esq, n->f_dret->f_dret))
        {
            n->f_esq->info = token(token::RESTA);
            esborra_arbre(n->f_esq->f_esq);
            n->f_esq->f_esq = n->f_esq->f_dret;
            n->f_esq->f_dret = n->f_dret->f_esq;
            n->info = token(token::MULTIPLICACIO);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(h) E ∗ E1 − E ∗ E2 → (E1 − E2) ∗ E
        else if (n->f_esq->info.tipus() == token::MULTIPLICACIO and n->f_dret->info.tipus() == token::MULTIPLICACIO and arbres_iguals(n->f_esq->f_esq, n->f_dret->f_esq))
        {
            n->f_esq->info = token(token::RESTA);
            esborra_arbre(n->f_esq->f_esq);
            n->f_esq->f_esq = n->f_esq->f_dret;
            n->f_esq->f_dret = n->f_dret->f_dret;
            n->info = token(token::MULTIPLICACIO);
            node *aux = n->f_dret->f_esq;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(i) E1/E − E2/E → (E1 − E2)/E
        else if (n->f_esq->info.tipus() == token::DIVISIO and n->f_dret->info.tipus() == token::DIVISIO and arbres_iguals(n->f_esq->f_dret, n->f_dret->f_dret))
        {
            n->f_esq->info = token(token::RESTA);
            esborra_arbre(n->f_esq->f_dret);
            n->f_esq->f_dret = n->f_dret->f_esq;
            n->info = token(token::DIVISIO);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
    }
    return false;
}

bool expressio::simplifica_canvi_de_signe(node *n)
{
    // Regles del canvi de signe i del signe positiu
    //(a) −(−E) → E
    if (n->info.tipus() == token::CANVI_DE_SIGNE and n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
    {
        n->info = n->f_dret->f_dret->info;
        n->f_esq = n->f_dret->f_dret->f_esq;
        node *aux1 = n->f_dret;
        node *aux2 = n->f_dret->f_dret;
        n->f_dret = n->f_dret->f_dret->f_dret;
        delete aux1;
        delete aux2;
        return true;
    }
    //(b) +E → E
    else if (n->info.tipus() == token::SIGNE_POSITIU)
    {
        n->info = n->f_dret->info;
        n->f_esq = n->f_dret->f_esq;
        node *aux = n->f_dret;
        n->f_dret = n->f_dret->f_dret;
        delete aux;
        return true;
    }
    //(c) Per qualsevol constant c (entera, racional o real), -c
    else if (n->info.tipus() == token::CANVI_DE_SIGNE and (n->f_dret->info.tipus() == token::CT_ENTERA or n->f_dret->info.tipus() == token::CT_RACIONAL or n->f_dret->info.tipus() == token::CT_REAL))
    {
        if (n->f_dret->info.tipus() == token::CT_ENTERA)
            n->info = token(-(n->f_dret->info.valor_enter()));
        else if (n->f_dret->info.tipus() == token::CT_RACIONAL)
            n->info = token(racional(-n->f_dret->info.valor_racional().num(), n->f_dret->info.valor_racional().denom()));
        else if (n->f_dret->info.tipus() == token::CT_REAL)
            n->info = token(-(n->f_dret->info.valor_real()));
        esborra_arbre(n->f_dret);
        n->f_dret = nullptr;
        return true;
    }
    return false;
}

bool expressio::simplifica_multiplicacio(node *n)
{
    // Regles de multiplicació
    if (n->info.tipus() == token::MULTIPLICACIO)
    {
        //(a) 1 ∗ E → E
        if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_esq->info.valor_enter() == 1)
        {
            n->info = n->f_dret->info;
            esborra_arbre(n->f_esq);
            node *aux = n->f_dret;
            n->f_esq = n->f_dret->f_esq;
            n->f_dret = n->f_dret->f_dret;
            delete aux;
            return true;
        }
        //(b) E ∗ 1 → E
        else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 1)
        {
            n->info = n->f_esq->info;
            esborra_arbre(n->f_dret);
            node *aux = n->f_esq;
            n->f_dret = n->f_esq->f_dret;
            n->f_esq = n->f_esq->f_esq;
            delete aux;
            return true;
        }
        //(c) 0 ∗ E → 0
        else if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_esq->info.valor_enter() == 0)
        {
            n->info = n->f_esq->info;
            esborra_arbre(n->f_esq);
            esborra_arbre(n->f_dret);
            n->f_esq = nullptr;
            n->f_dret = nullptr;
            return true;
        }
        //(d) E ∗ 0 → 0
        else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
        {
            n->info = n->f_dret->info;
            esborra_arbre(n->f_esq);
            esborra_arbre(n->f_dret);
            n->f_esq = nullptr;
            n->f_dret = nullptr;
            return true;
        }
        //(e) E ∗ E → Eˆ2
        else if (arbres_iguals(n->f_esq, n->f_dret))
        {
            n->info = token(token::EXPONENCIACIO);
            esborra_arbre(n->f_dret);
            n->f_dret = new node(token(2));
            return true;
        }
        //(f) E ∗ (−E′) → −(E ∗ E′)
        else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
        {
            n->info = token(token::CANVI_DE_SIGNE);
            n->f_dret->info = token(token::MULTIPLICACIO);
            n->f_dret->f_esq = n->f_esq;
            n->f_esq = nullptr;
            return true;
        }
        //(g) (−E) ∗ E′ → −(E ∗ E′)
        else if (n->f_esq->info.tipus() == token::CANVI_DE_SIGNE)
        {
            n->info = token(token::CANVI_DE_SIGNE);
            n->f_esq->info = token(token::MULTIPLICACIO);
            n->f_esq->f_esq = n->f_esq->f_dret;
            n->f_esq->f_dret = n->f_dret;
            n->f_dret = n->f_esq;
            n->f_esq = nullptr;
            return true;
        }
        //(h) E ∗ (1/E′) → E/E′
        else if (n->f_dret->info.tipus() == token::DIVISIO and n->f_dret->f_esq->info.tipus() == token::CT_ENTERA and n->f_dret->f_esq->info.valor_enter() == 1)
        {
            n->info = token(token::DIVISIO);
            esborra_arbre(n->f_dret->f_esq);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(i) (1/E′) ∗ E → E/E′
        else if (n->f_esq->info.tipus() == token::DIVISIO and n->f_esq->f_esq->info.tipus() == token::CT_ENTERA and n->f_esq->f_esq->info.valor_enter() == 1)
        {
            n->info = token(token::DIVISIO);
            esborra_arbre(n->f_esq->f_esq);
            n->f_esq->f_esq = n->f_dret;
            n->f_dret = n->f_esq->f_dret;
            node *aux = n->f_esq->f_esq;
            delete n->f_esq;
            n->f_esq = aux;
            return true;
        }
        //(j) E1ˆE ∗ E2ˆE → (E1 ∗ E2)ˆE
        else if (n->f_esq->info.tipus() == token::EXPONENCIACIO and n->f_dret->info.tipus() == token::EXPONENCIACIO and arbres_iguals(n->f_esq->f_dret, n->f_dret->f_dret))
        {
            n->f_esq->info = token(token::MULTIPLICACIO);
            esborra_arbre(n->f_esq->f_dret);
            n->f_esq->f_dret = n->f_dret->f_esq;
            n->info = token(token::EXPONENCIACIO);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(k) EˆE1 ∗ EˆE2 → Eˆ(E1 + E2)
        else if (n->f_esq->info.tipus() == token::EXPONENCIACIO and n->f_dret->info.tipus() == token::EXPONENCIACIO and arbres_iguals(n->f_esq->f_esq, n->f_dret->f_esq))
        {
            n->info = token(token::EXPONENCIACIO);
            esborra_arbre(n->f_dret->f_esq);
            n->f_dret->f_esq = n->f_esq->f_dret;
            n->f_dret->info = token(token::SUMA);
            node *aux = n->f_esq->f_esq;
            delete n->f_esq;
            n->f_esq = aux;
            return true;
        }
        //(l) exp(E) ∗ exp(E′) → exp(E + E′)
        else if (n->f_esq->info.tipus() == token::EXP and n->f_dret->info.tipus() == token::EXP)
        {
            n->info = token(token::EXP);
            n->f_dret->info = token(token::SUMA);
            n->f_dret->f_esq = n->f_esq->f_dret;
            delete n->f_esq;
            n->f_esq = nullptr;
            return true;
        }
    }
    return false;
}

bool expressio::simplifica_divisio(node *n)
{
    // Regles de divisió
    if (n->info.tipus() == token::DIVISIO)
    {
        //(a) E/0 és un error
        if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
            // Si c2 és una constant negativa, es llença l'error "Negatiu elevat a no enter".
            throw error(DivPerZero);
        //(b) 0/E → 0
        else if (n->f_esq->info.tipus() == token::CT_ENTERA and n->f_esq->info.valor_enter() == 0)
        {
            n->info = token(0);
            esborra_arbre(n->f_esq);
            esborra_arbre(n->f_dret);
            n->f_esq = nullptr;
            n->f_dret = nullptr;
            return true;
        }
        //(c) E/1 → E
        else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 1)
        {
            n->info = n->f_esq->info;
            esborra_arbre(n->f_dret);
            node *aux = n->f_esq;
            n->f_dret = n->f_esq->f_dret;
            n->f_esq = n->f_esq->f_esq;
            delete aux;
            return true;
        }
        //(d) E/E → 1
        else if (arbres_iguals(n->f_esq, n->f_dret))
        {
            n->info = token(1);
            esborra_arbre(n->f_esq);
            esborra_arbre(n->f_dret);
            n->f_esq = nullptr;
            n->f_dret = nullptr;
            return true;
        }
        //(e) E/(−E′) → −(E/E′)
        else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
        {
            n->info = token(token::CANVI_DE_SIGNE);
            n->f_dret->info = token(token::DIVISIO);
            n->f_dret->f_esq = n->f_esq;
            n->f_esq = nullptr;
            return true;
        }
        //(f) E/(1/E′) → E ∗ E′
        else if (n->f_dret->info.tipus() == token::DIVISIO and n->f_dret->f_esq->info.tipus() == token::CT_ENTERA and n->f_dret->f_esq->info.valor_enter() == 1)
        {
            n->info = token(token::MULTIPLICACIO);
            esborra_arbre(n->f_dret->f_esq);
            node *aux = n->f_dret;
            n->f_dret = n->f_dret->f_dret;
            delete aux;
            return true;
        }
        //(g) E1ˆE/E2ˆE → (E1/E2)ˆE
        else if (n->f_esq->info.tipus() == token::EXPONENCIACIO and n->f_dret->info.tipus() == token::EXPONENCIACIO and arbres_iguals(n->f_esq->f_dret, n->f_dret->f_dret))
        {
            n->f_esq->info = token(token::DIVISIO);
            esborra_arbre(n->f_esq->f_dret);
            n->f_esq->f_dret = n->f_dret->f_esq;
            n->info = token(token::EXPONENCIACIO);
            node *aux = n->f_dret->f_dret;
            delete n->f_dret;
            n->f_dret = aux;
            return true;
        }
        //(h) EˆE1/EˆE2 → Eˆ(E1 − E2)
        else if (n->f_esq->info.tipus() == token::EXPONENCIACIO and n->f_dret->info.tipus() == token::EXPONENCIACIO and arbres_iguals(n->f_esq->f_esq, n->f_dret->f_esq))
        {
            n->info = token(token::EXPONENCIACIO);
            esborra_arbre(n->f_dret->f_esq);
            n->f_dret->f_esq = n->f_esq->f_dret;
            n->f_dret->info = token(token::RESTA);
            node *aux = n->f_esq->f_esq;
            delete n->f_esq;
            n->f_esq = aux;
            return true;
        }
        //(i) exp(E)/exp(E′) → exp(E − E′)
        else if (n->f_esq->info.tipus() == token::EXP and n->f_dret->info.tipus() == token::EXP)
        {
            n->info = token(token::EXP);
            n->f_dret->info = token(token::RESTA);
            n->f_dret->f_esq = n->f_esq->f_dret;
            delete n->f_esq;
            n->f_esq = nullptr;
            return true;
        }
    }
    return false;
}

bool expressio::simplifica_exponenciacio(node *n)
{
    // Regles de exponenciació
    if (n->info.tipus() == token::EXPONENCIACIO)
    {
        //(a) Si c2 és una constant de tipus enter, o si c1 i c2 son constants reals llavors
        if (es_calcul_exponenciacio(n))
        {
            int exponent;
            if (n->f_dret->info.tipus() == token::CT_ENTERA)
                exponent = n->f_dret->info.valor_enter();
            if (n->f_esq->info.tipus() == token::CT_ENTERA)
            {
                int base = n->f_esq->info.valor_enter();
                if (exponent >= 0)
                    n->info = token((int)pow(base, exponent));
                else
                    n->info = token(racional(1, pow(base, abs(exponent))));
            }
            else if (n->f_esq->info.tipus() == token::CT_RACIONAL)
            {
                racional base = n->f_esq->info.valor_racional();
                if (exponent >= 0)
                    n->info = token(racional(pow(base.num(), exponent), pow(base.denom(), exponent)));
                else
                    n->info = token(racional(pow(base.denom(), abs(exponent)), pow(base.num(), abs(exponent))));
            }
            else if (n->f_esq->info.tipus() == token::CT_REAL)
            {
                double base = n->f_esq->info.valor_real();
                if (n->f_dret->info.tipus() == token::CT_REAL)
                {
                    double exponent = n->f_dret->info.valor_real();
                    n->info = token(pow(base, exponent));
                }
                else if (n->f_dret->info.tipus() == token::CT_ENTERA)
                    n->info = token(pow(base, exponent));
            }
            esborra_arbre(n->f_esq);
            esborra_arbre(n->f_dret);
            n->f_esq = nullptr;
            n->f_dret = nullptr;
            return true;
        }
        // Es produeix un error si c1 és una constant negativa (entera, racional o real) i c2 no és una constant entera.
        else if (es_negatiu(n->f_esq->info) and n->f_dret->info.tipus() != token::CT_ENTERA)
            // Si c1 és una constant negativa, es llença l'error "Negatiu elevat a no enter".
            throw error(NegatElevNoEnter);
        //(b) Eˆ0 → 1
        else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
        {
            n->info = token(1);
            esborra_arbre(n->f_esq);
            esborra_arbre(n->f_dret);
            n->f_esq = nullptr;
            n->f_dret = nullptr;
            return true;
        }
        //(c) Eˆ1 → E
        else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 1)
        {
            n->info = n->f_esq->info;
            esborra_arbre(n->f_dret);
            node *aux = n->f_esq;
            n->f_dret = n->f_esq->f_dret;
            n->f_esq = n->f_esq->f_esq;
            delete aux;
            return true;
        }
        //(d) Eˆ − E′ → 1/(EˆE′)
        else if (n->f_dret->info.tipus() == token::CANVI_DE_SIGNE)
        {
            n->info = token(token::DIVISIO);
            n->f_dret->info = token(token::EXPONENCIACIO);
            n->f_dret->f_esq = n->f_esq;
            n->f_esq = new node(token(1));
            return true;
        }
        //(e) (E1ˆE2)ˆE3 → E1ˆ(E2 ∗ E3)
        else if (n->f_esq->info.tipus() == token::EXPONENCIACIO)
        {
            node *aux1 = n->f_esq->f_esq;
            node *aux2 = n->f_esq->f_dret;
            node *aux3 = n->f_dret;
            delete n->f_esq;
            n->f_esq = aux1;
            n->f_dret = new node(token(token::MULTIPLICACIO));
            n->f_dret->f_esq = aux2;
            n->f_dret->f_dret = aux3;
            return true;
        }
        //(f) (exp(E))ˆE′ → exp(E ∗ E′)
        else if (n->f_esq->info.tipus() == token::EXP)
        {
            n->info = token(token::EXP);
            n->f_esq->f_esq = n->f_dret;
            n->f_dret = new node(token(token::MULTIPLICACIO));
            n->f_dret->f_dret = n->f_esq->f_esq;
            n->f_dret->f_esq = n->f_esq->f_dret;
            delete n->f_esq;
            n->f_esq = nullptr;
            return true;
        }
    }
    return false;
}

bool expressio::simplifica_radicacio(node *n)
{
    // Regles de radicació (funció sqrt)
    if (n->info.tipus() == token::SQRT)
    {
        //(a) sqrt(c) → error, per qualsevol constant entera, racional o real c < 0
        if (es_negatiu(n->f_dret->info))
            // Si c és una constant negativa, es llença l'error "Arrel quadrada de negatiu".
            throw error(SqrtDeNegatiu);
        //(b) sqrt(0) → 0
        else if (n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
        {
            n->info = n->f_dret->info;
            esborra_arbre(n->f_esq);
            esborra_arbre(n->f_dret);
            n->f_esq = nullptr;
            n->f_dret = nullptr;
            return true;
        }
        //(c) Si la funció sqrt està aplicada sobre una constant en coma flotant es calcula d’immediat el seu resultat usant la funció corresponent de la biblioteca matemàtica de C++, i en concret es pendrà el valor positiu (p.e. tant 3.0 com -3.0 són valors correctes per sqrt(9.0), però ens quedarem amb el primer).
        else if (n->f_dret->info.tipus() == token::CT_REAL)
        {
            n->info = token(sqrt(n->f_dret->info.valor_real()));
            esborra_arbre(n->f_esq);
            esborra_arbre(n->f_dret);
            n->f_esq = nullptr;
            n->f_dret = nullptr;
            return true;
        }
        ///(d) sqrt(E) → Eˆ1/2 (aquí, una meitat es representa com constant racional, no com una expressió amb tres tokens)
        else
        {
            n->info = token(token::EXPONENCIACIO);
            n->f_esq = n->f_dret;
            n->f_dret = new node(token(racional(1, 2)));
            return true;
        }
    }
    return false;
}

bool expressio::simplifica_exponencial_i_logaritme(node *n)
{
    // Regles de les funcions exponencial i logaritme (natural)
    //(a) exp(0) → 1
    if (n->info.tipus() == token::EXP and n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 0)
    {
        n->info = token(1);
        esborra_arbre(n->f_esq);
        esborra_arbre(n->f_dret);
        n->f_esq = nullptr;
        n->f_dret = nullptr;
        return true;
    }
    //(b) exp(1) → e
    else if (n->info.tipus() == token::EXP and n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 1)
    {
        n->info = token(token::CT_E);
        esborra_arbre(n->f_esq);
        esborra_arbre(n->f_dret);
        n->f_esq = nullptr;
        n->f_dret = nullptr;
        return true;
    }
    //(c) exp(log(E)) → E
    else if (n->info.tipus() == token::EXP and n->f_dret->info.tipus() == token::LOG)
    {
        node *aux = n->f_dret->f_dret;
        delete n->f_dret;
        n->info = token(aux->info);
        n->f_dret = aux->f_dret;
        esborra_arbre(n->f_esq);
        n->f_esq = aux->f_esq;
        delete aux;
        return true;
    }
    //(d) log(c) → error, per qualsevol constant entera, racional o real c ≤ 0
    else if (n->info.tipus() == token::LOG and es_negatiu(n->f_dret->info))
        // Si c és una constant negativa, es llença l'error "Logaritme de no positiu".
        throw error(LogDeNoPositiu);
    //(e) log(1) → 0
    else if (n->info.tipus() == token::LOG and n->f_dret->info.tipus() == token::CT_ENTERA and n->f_dret->info.valor_enter() == 1)
    {
        n->info = token(0);
        esborra_arbre(n->f_esq);
        esborra_arbre(n->f_dret);
        n->f_esq = nullptr;
        n->f_dret = nullptr;
        return true;
    }
    //(f) log(e) → 1
    else if (n->info.tipus() == token::LOG and n->f_dret->info.tipus() == token::CT_E)
    {
        n->info = token(1);
        esborra_arbre(n->f_esq);
        esborra_arbre(n->f_dret);
        n->f_esq = nullptr;
        n->f_dret = nullptr;
        return true;
    }
    //(g) log(exp(E)) → E
    else if (n->info.tipus() == token::LOG and n->f_dret->info.tipus() == token::EXP)
    {
        node *aux = n->f_dret->f_dret;
        n->info = token(aux->info);
        delete n->f_dret;
        n->f_dret = aux->f_dret;
        esborra_arbre(n->f_esq);
        n->f_esq = aux->f_esq;
        delete aux;
        return true;
    }
    //(h) log(E ∗ E′)) → log(E) + log(E′)
    else if (n->info.tipus() == token::LOG and n->f_dret->info.tipus() == token::MULTIPLICACIO)
    {
        n->info = token(token::SUMA);
        n->f_dret->info = token(token::LOG);
        n->f_esq = new node(token(token::LOG));
        n->f_esq->f_dret = n->f_dret->f_esq;
        n->f_dret->f_esq = nullptr;
        return true;
    }
    //(i) log(E/E′)) → log(E) − log(E′)
    else if (n->info.tipus() == token::LOG and n->f_dret->info.tipus() == token::DIVISIO)
    {
        n->info = token(token::RESTA);
        n->f_dret->info = token(token::LOG);
        n->f_esq = new node(token(token::LOG));
        n->f_esq->f_dret = n->f_dret->f_esq;
        n->f_dret->f_esq = nullptr;
        return true;
    }
    // (j) log(EˆE′)) → E′ ∗ log(E)
    else if (n->info.tipus() == token::LOG and n->f_dret->info.tipus() == token::EXPONENCIACIO)
    {
        n->info = token(token::MULTIPLICACIO);
        n->f_dret->info = token(token::LOG);
        n->f_esq = n->f_dret->f_dret;
        n->f_dret->f_dret = n->f_dret->f_esq;
        n->f_dret->f_esq = nullptr;
        return true;
    }
    //(k) Si la funció exp o log estan aplicades sobre una constant en coma flotant es calcula d’immediato el seu resultat usant les funcions corresponents de la biblioteca matemàtica de C++.
    else if ((n->info.tipus() == token::EXP or n->info.tipus() == token::LOG) and n->f_dret->info.tipus() == token::CT_REAL)
    {
        if (n->info.tipus() == token::EXP)
            n->info = token(exp(n->f_dret->info.valor_real()));
        else
            n->info = token(log(n->f_dret->info.valor_real()));
        esborra_arbre(n->f_esq);
        esborra_arbre(n->f_dret);
        n->f_esq = nullptr;
        n->f_dret = nullptr;
        return true;
    }
    return false;
}

bool expressio::simplifica_evalf(node *n)
{
    // L’operació evalf retorna quelcom diferent depenent al que s’estigui aplicant
    if (n->info.tipus() == token::EVALF)
    {
        // Si s’aplica sobre una constant en coma flotant torna la pròpia constant.
        if (n->f_dret->info.tipus() == token::CT_REAL)
        {
            n->info = token(n->f_dret->info.valor_real());
            esborra_arbre(n->f_dret);
            n->f_dret = nullptr;
            return true;
        }
        // Si s’aplica sobre un enter realitza la conversió,
        else if (n->f_dret->info.tipus() == token::CT_ENTERA)
        {
            n->info = token((double)n->f_dret->info.valor_enter());
            esborra_arbre(n->f_dret);
            n->f_dret = nullptr;
            return true;
        }
        // i si s’aplica sobre un racional es calcula el quocient real entre el numerador i el denominador.
        else if (n->f_dret->info.tipus() == token::CT_RACIONAL)
        {
            n->info = token((double)n->f_dret->info.valor_racional().num() / n->f_dret->info.valor_racional().denom());
            esborra_arbre(n->f_dret);
            n->f_dret = nullptr;
            return true;
        }
        // Si s’aplica sobre la constant e retorna el valor 2.718281828.
        else if (n->f_dret->info.tipus() == token::CT_E)
        {
            n->info = token(2.718281828);
            esborra_arbre(n->f_dret);
            n->f_dret = nullptr;
            return true;
        }
        // Si s’aplica sobre una variable x que no té valor assignat retorna la pròpia variable
        else if (n->f_dret->info.tipus() == token::VARIABLE or n->f_dret->info.tipus() == token::VAR_PERCENTATGE)
        {
            if (n->f_dret->info.tipus() == token::VARIABLE)
                n->info = token(n->f_dret->info.identificador_variable());
            else
                n->info = n->f_dret->info;
            esborra_arbre(n->f_dret);
            n->f_dret = nullptr;
            return true;
        }
        // La seva aplicació és recursiva: evalf(E op E′) → evalf(E) op evalf(E′) si op és una operació binària
        else if (es_operador_binari(n->f_dret->info))
        {
            node *aux = n->f_dret->f_esq;
            node *aux2 = n->f_dret->f_dret;
            n->info = n->f_dret->info;
            n->f_esq = new node(token(token::EVALF));
            n->f_esq->f_dret = aux;
            n->f_dret->info = token(token::EVALF);
            n->f_dret->f_dret = aux2;
            n->f_dret->f_esq = nullptr;
            return true;
        }
        // evalf(f(E)) → f(evalf(E)) per una funció f qualsevol exceptuant la pròpia evalf
        else if (es_funcio(n->f_dret->info) and n->f_dret->info.tipus() != token::EVALF)
        {
            n->info = n->f_dret->info;
            n->f_dret->info = token(token::EVALF);
            return true;
        }
    }
    return false;
}
