#include "variables.hpp"

// COST: O(1), ja que només està inicialitzant una variable amb un valor predeterminat.
variables::variables() throw(error)
{
   _arrel = nullptr;
}

// COST: O(n), ja que utilitza la funció "copiar_arbre" té cost O(n).
variables::variables(const variables &v) throw(error)
{
   _arrel = copiar_arbre(v._arrel);
}

// COST: O(n), ja que la funció "esborra_arbre" i "copiar_arbre" tenen cost O(n).
variables &variables::operator=(const variables &v) throw(error)
{
   if (this != &v)
   {
      esborra_arbre(_arrel);
      _arrel = copiar_arbre(v._arrel);
   }
   return *this;
}

// COST: O(n), ja que la funció "esborra_arbre" té cost O(n).
variables::~variables() throw(error)
{
   esborra_arbre(_arrel);
   _arrel = nullptr;
}

// COST: O(log n), ja que utilitza l'algorisme d'inserció en un AVL per afegir la nova variable al conjunt.
void variables::assign(const string &v, const expressio &e) throw(error)
{
   _arrel = insereix_avl(_arrel, v, e);
}

// COST: O(log n), ja que utilitza l'algorisme d'eliminació en un AVL per afegir la nova variable al conjunt.
void variables::unassign(const string &v) throw()
{
   _arrel = elimina_avl(_arrel, v);
}

// COST: O(log n), ja que utilitza l'algorisme d'AVL per cercar la variable amb nom "lv".
expressio variables::valor(const string &lv) const throw(error)
{
   node *n = consulta_avl(_arrel, lv);
   if (n != nullptr)
      return n->_v;
   else
      return expressio(token(token::NULLTOK));
}

// COST: O(n), ja que recorre totes les dades de l'arbre.
void variables::dump(list<string> &l) const throw(error)
{
   recorrer_arbre(_arrel, l);
}

// COST: O(n), ja que la funció "copiar_arbre" té cost O(n).
variables::node::node(const string &k, const expressio &v, const int &altura, node *esq, node *dret)
{
   _k = k;
   _v = v;
   _esq = copiar_arbre(esq);
   _dret = copiar_arbre(dret);
   _altura = altura;
}

// COST: O(1), ja que només fa una comparació.
int variables::altura(node *n)
{
   if (n == nullptr)
      return 0;
   return n->_altura;
}

// COST: O(1), ja que la funció "altura" té cost O(1).
int variables::factor_equilibri(node *n)
{
   if (n == nullptr)
      return 0;
   return altura(n->_esq) - altura(n->_dret);
}

// COST: O(1), ja que només efectua operacions bàsiques.
variables::node *variables::rotacio_dreta(node *y)
{
   node *x = y->_esq;
   node *T2 = x->_dret;
   // Realitzem la rotació
   x->_dret = y;
   y->_esq = T2;
   // Actualitzem les altures
   y->_altura = std::max(altura(y->_esq), altura(y->_dret)) + 1;
   x->_altura = std::max(altura(x->_esq), altura(x->_dret)) + 1;
   // Retornem la nova arrel
   return x;
}

// COST: O(1), ja que només efectua operacions bàsiques.
variables::node *variables::rotacio_esquerra(node *x)
{
   node *y = x->_dret;
   node *T2 = y->_esq;
   // Realitzem la rotació
   y->_esq = x;
   x->_dret = T2;
   // Actualitzem les altures
   x->_altura = std::max(altura(x->_esq), altura(x->_dret)) + 1;
   y->_altura = std::max(altura(y->_esq), altura(y->_dret)) + 1;
   // Retornem la nova arrel
   return y;
}

// COST: O(log n), ja que utilitza un algorisme de cerca binària per inserir la parella clau-valor a l'arbre i, a més a més, manté l'equilibri de l'arbre mitjançant rotacions.
variables::node *variables::insereix_avl(node *n, const string &k, const expressio &v)
{
   // 1. Fem la inserció a un AVL
   if (n == nullptr)
      return new node(k, v);
   else if (k < n->_k)
      n->_esq = insereix_avl(n->_esq, k, v);
   else if (k > n->_k)
      n->_dret = insereix_avl(n->_dret, k, v);
   else
   {
      n->_v = v;
      return n;
   }
   // 2. Actualitzem l’altura
   n->_altura = std::max(altura(n->_esq), altura(n->_dret)) + 1;
   // 3. Obtenim el factor d’equilibri per veure si està balancejat
   int fe = factor_equilibri(n);
   // Cas EE
   if (fe > 1 and k < n->_esq->_k)
      return rotacio_dreta(n);
   // Cas DD
   if (fe < -1 and k > n->_dret->_k)
      return rotacio_esquerra(n);
   // Cas ED
   if (fe > 1 and k > n->_esq->_k)
   {
      n->_esq = rotacio_esquerra(n->_esq);
      return rotacio_dreta(n);
   }
   // Cas DE
   if (fe < -1 and k < n->_dret->_k)
   {
      n->_dret = rotacio_dreta(n->_dret);
      return rotacio_esquerra(n);
   }
   return n;
}

// COST: O(log n), ja que utilitza un bucle "while" per recórrer els nodes de l'arbre a l'esquerra.
variables::node *variables::node_valor_minim(node *n)
{
   node *actual = n;
   while (actual->_esq != nullptr)
      actual = actual->_esq;
   return actual;
}

// COST: O(log n), ja que utilitza un algorisme de cerca binària per eliminar la parella clau-valor a l'arbre i, a més a més, manté l'equilibri de l'arbre mitjançant rotacions.
variables::node *variables::elimina_avl(node *n, const string &k)
{
   // 1. Fem l'eliminació a un AVL
   if (n == nullptr)
      return n;
   else if (k < n->_k)
      n->_esq = elimina_avl(n->_esq, k);
   else if (k > n->_k)
      n->_dret = elimina_avl(n->_dret, k);
   else
   {
      if ((n->_esq == nullptr) or (n->_dret == nullptr))
      {
         node *aux;
         if (n->_esq)
            aux = n->_esq;
         else
            aux = n->_dret;
         if (aux == nullptr)
         {
            aux = n;
            n = nullptr;
         }
         else
            *n = *aux;
         esborra_arbre(aux);
      }
      else
      {
         node *aux = node_valor_minim(n->_dret);
         n->_k = aux->_k;
         n->_v = aux->_v;
         n->_dret = elimina_avl(n->_dret, aux->_k);
      }
   }
   if (n == nullptr)
      return n;
   // 2. Actualitzem l’altura
   n->_altura = std::max(altura(n->_esq), altura(n->_dret)) + 1;
   // 3. Obtenim el factor d’equilibri per veure si està balancejat
   int fe = factor_equilibri(n);
   // Cas EE
   if (fe > 1 and factor_equilibri(n->_esq) >= 0)
      return rotacio_dreta(n);
   // Cas DD
   if (fe < -1 and factor_equilibri(n->_dret) <= 0)
      return rotacio_esquerra(n);
   // Cas ED
   if (fe > 1 and factor_equilibri(n->_esq) < 0)
   {
      n->_esq = rotacio_esquerra(n->_esq);
      return rotacio_dreta(n);
   }
   // Cas DE
   if (fe < -1 and factor_equilibri(n->_dret) > 0)
   {
      n->_dret = rotacio_dreta(n->_dret);
      return rotacio_esquerra(n);
   }
   return n;
}

// COST: O(log n), ja que utilitza un algorisme de cerca binària.
variables::node *variables::consulta_avl(node *n, const string &k)
{
   while (n != nullptr and k != n->_k)
   {
      if (k < n->_k)
         n = n->_esq;
      else // k > n->_k
         n = n->_dret;
   }
   return n;
}

// COST: O(n), ja que visita tots els nodes de l'arbre.
void variables::recorrer_arbre(node *n, list<string> &lt)
{
   if (n != nullptr)
   {
      recorrer_arbre(n->_esq, lt);
      lt.push_back(n->_k);
      recorrer_arbre(n->_dret, lt);
   }
}

// COST: O(n), ja que recorre tots els nodes de l'arbre donat.
variables::node *variables::copiar_arbre(node *n)
{
   if (n != nullptr)
   {
      node *nn = new node(n->_k, n->_v, n->_altura);
      nn->_esq = copiar_arbre(n->_esq);
      nn->_dret = copiar_arbre(n->_dret);
      return nn;
   }
   return nullptr;
}

// COST: O(n), ja que recorre tots els nodes de l'arbre passat com a paràmetre.
void variables::esborra_arbre(node *n)
{
   if (n != nullptr)
   {
      esborra_arbre(n->_esq);
      esborra_arbre(n->_dret);
      delete n;
   }
}
