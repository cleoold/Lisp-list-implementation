#
#   lisp list (cons cells) in python
#
#   (c) cos in Aug 2019



from typing import Union, Callable

Number = Union[int, float]


# utilities

def __typecheck_Number__(a, b):
    if not (isinstance(a, int) or isinstance(a, float)) or \
        not (isinstance(b, int) or isinstance(b, float)):
        raise ValueError()

def __typecheck_String__(a, b):
    if not (isinstance(a, str) and isinstance(b, str)):
        raise ValueError()

def add(a: Number, b: Number) -> Number:
    __typecheck_Number__(a, b)
    return a + b

def sub(a: Number, b: Number) -> Number:
    __typecheck_Number__(a, b)
    return a - b

def mul(a: Number, b: Number) -> Number:
    __typecheck_Number__(a, b)
    return a * b

def div(a: Number, b: Number) -> Number:
    __typecheck_Number__(a, b)
    return a / b

def string_append(a: str, b:str) -> str:
    __typecheck_String__(a, b)
    return a + b



# function operations

def curry1(f: Callable) -> Callable:
    'requires the argument count for f is 1'
    def g(**kwargs):
        def h(arg1):
            return f(arg1, **kwargs)
        return h
    return g

def curry2(f: Callable) -> Callable:
    'requires the argument count for f is 2'
    def g(arg1, **kwargs):
        def h(arg2):
            return f(arg1, arg2, **kwargs)
        return h
    return g

def curry(f: Callable) -> Callable:
    'requires the argument count for f is at least 2'
    def g(arg2, *args_rest, **kwargs):
        def h(arg1):
            return f(arg1, arg2, *args_rest, **kwargs)
        return h
    return g

def compose(f: Callable, g: Callable) -> Callable:
    def fg(*args, **kwargs):
        return f(g(*args, **kwargs))
    return fg

add1 = curry(add)(1)



# lisp list definition and functions

class cons(tuple):
    def __new__(cls, *arg):
        if len(arg) != 0:
            if len(arg) != 2:
                raise ValueError('arity mismatch')
            if not isinstance(arg[1], cls):
                raise ValueError('second argument must be a list')
        self = super().__new__(cls, arg)
        return self

    def first(self): return self[0]

    def rest(self): return self[1]

    car = first
    cdr = rest

    def isnull(self) -> bool:
        return len(self) == 0

    def ref(self, index: int):
        if index == 1:
            return self.first()
        else:
            return self.rest().ref(index-1)

    def last(self):
        def last1(lst: cons):
            if lst.rest().isnull():
                return lst.first()
            else:
                return last1(lst.rest())
        return last1(self)

    def reverse(self):
        def reverse1(lst: cons, reversedd: cons):
            if lst.isnull():
                return reversedd
            else:
                return reverse1(lst.rest(), cons(lst.first(), reversedd))
        return reverse1(self, cons())

    def __eq__(self, other) -> bool:
        return listsEqual1(self, other)

    def __add__(self, other):
        raise NotImplementedError

    def __sub__(self, other):
        raise NotImplementedError

    def quasi_repr(self) -> str:
        'returns the quasiquote representation of the list'
        if self.isnull():
            return '\'()'
        def reprB(item) -> str:
            if isinstance(item, cons):
                if item.isnull():
                    return '()'
                else:
                    return string_append('(', repr1(item))
            else:
                return str(item)
        def repr1(lst: cons) -> str:
            if lst.rest().isnull():
                return string_append(reprB(lst.first()), ')')
            else:
                return string_append(reprB(lst.first()), 
            string_append(', ', repr1(lst.rest()) ))
        return string_append('\'(', repr1(self))

    def length(self) -> int:
        def length1(lst: cons, n: int) -> int:
            if lst.isnull():
                return n
            else:
                return length1(lst.rest(), n+1)
        return length1(self, 0)

    def to_vector(self, store: list) -> list:
        'converts it to a normal list'
        def to_vector1(lst: cons):
            if not lst.isnull():
                store.append(lst.first())
                return to_vector1(lst.rest())
        return to_vector1(self)

    def apply_to_sup(self, f: Callable):
        '''apply this cons list to a function that takes arguments of this length

        (pythonic)
        '''
        temp = list()
        self.to_vector(temp)
        return f(*temp)

    def apply_to(self, f: Callable):
        '''apply this cons list to a function that takes arguments of this length

        (first peel the function until it becomes single-variate, then fill in the args)'''
        if self.isnull():
            return f()
        def reduce_function(f1: Callable, n: int) -> Callable:
            if n == 1:
                return f1
            else:
                return reduce_function(curry(f1), n-1)
        def fill_args(f1: Callable, arg_lst_r: cons):
            if arg_lst_r.rest().isnull():
                return f1(arg_lst_r.first())
            else:
                return fill_args(f1(arg_lst_r.first()), arg_lst_r.rest())
        return fill_args(
                reduce_function(f, self.length()), 
                self.reverse())

null = cons()





def vector_to_cons(vec: list) -> cons:
    'converts a normal python list to a cons list'
    length: int = len(vec)
    def vector_to_cons1(count: int) -> cons:
        if count == length:
            return cons()
        else:
            return cons(vec[count], vector_to_cons1(count+1))
    return vector_to_cons1(0)

def lispList(*args) -> cons:
    'usage: lispList(1,2) => cons(1, cons(2, cons()))'
    return vector_to_cons(args)





def lispFilter(cond: Callable, lst: cons) -> cons:
    'keeps only items that meets condition'
    if lst.isnull():
        return lst
    elif cond(lst.first()):
        return cons(lst.first(), lispFilter(cond, lst.rest()))
    else:
        return lispFilter(cond, lst.rest())





def lispMap1(f: Callable, alst: cons) -> cons:
    'map function, single-variate'
    if alst.isnull():
        return cons()
    else:
        return cons(f(alst.first()),
                    lispMap1(f, alst.rest()))


def lispMap(f: Callable, *varlists: cons) -> cons:
    'map function, multi-variate'
    def map1(f1, alst: cons) -> cons:
        if alst.isnull():
            return cons()
        else:
            return cons(f1(alst.first()),
                        map1(f1, alst.rest()))
    if len(varlists) == 1:
        return map1(f, varlists[0])
    def var_map(lists: cons):
        if lists.first().isnull():
            return cons()
        else:
            return cons(map1(cons.first, lists).apply_to(f),
                        var_map(map1(cons.rest, lists)))
    return var_map(vector_to_cons(varlists))




def listsEqual1(lst1: cons, lst2: cons) -> bool:
    'determine the equalness of two lists'
    if lst1.isnull() and lst2.isnull():
        return True
    elif (lst1.isnull() and not lst2.isnull()) \
        or (not lst1.isnull() and lst2.isnull()):
        return False
    elif lst1.first() == lst2.first():
        return True and listsEqual1(lst1.rest(), lst2.rest())
    else:
        return False






def lispFoldr1(f: Callable, end, lst: cons):
    'fold right function, single-variate'
    if lst.isnull():
        return end
    else:
        return f(lst.first(), lispFoldr1(f, end, lst.rest()))

def lispFoldr(f: Callable, end, *varlists: cons):
    'fold right function, multi-variate'
    def mapadd(f1: Callable, lst: cons, last) -> cons:
        if lst.isnull():
            return cons(last, cons())
        else:
            return cons(f1(lst.first()), mapadd(f1, lst.rest(), last))
    def var_foldr(lists: cons):
        if lists.first().isnull():
            return end
        else:
            return mapadd(cons.first, lists, 
                    var_foldr(lispMap(cons.rest, lists)) ).apply_to(f)
    return var_foldr(vector_to_cons(varlists))






def RacketFoldl1(f: Callable, acc, lst: cons):
    'RACKET fold left function, single-variate'
    if lst.isnull():
        return acc
    else:
        return RacketFoldl1(f, f(lst.first(), acc), lst.rest())

def RacketFoldl(f: Callable, acc, *varlists: cons):
    'RACKET fold left function, multi-variate'
    def mapadd(f1: Callable, lst: cons, last) -> cons:
        if lst.isnull():
            return cons(last, cons())
        else:
            return cons(f1(lst.first()), mapadd(f1, lst.rest(), last))
    def var_foldl(lists: cons, acc):
        if lists.first().isnull():
            return acc
        else:
            return var_foldl(lispMap(cons.rest, lists), 
                                mapadd(cons.first, lists, acc).apply_to(f))
    return var_foldl(vector_to_cons(varlists), acc)
