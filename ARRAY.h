/////2222////////////
/////1111////////////
/// used in the canny edge.
#if !defined(ARRAY_H)  
#define ARRAY_H  

//------------------------------------------  
// 4-dimensional dynamic memory allocation  
// template class : Array4D  
// 1996.10.12 by CHEN Ching-Han  
//------------------------------------------  

template <class T>  
class Array4D  
{  
protected:  
    void alloc();  
    void free();  
public:  
    int nr;  
    int nc;  
    int nd;  
    int nw;  
    T ****m;  
    Array4D(){m=0;}  
    Array4D(int r,int c,int d, int w){nr=r;nc=c;nd=d;nw=w;alloc();}  
    void Initialize(int row, int col, int dep, int wid);  
    void Finish(){free();}  
    Array4D<T> operator=(Array4D<T>& t);  
    T operator()(int x,int y,int z,int w){return m[x][y][z][w];}  
    ~Array4D(){}  
};  


//------------------------------------------  
// 3-dimensional dynamic memory allocation  
// template class : Array3D  
// 1996.10.3 by CHEN Ching-Han  
//------------------------------------------  

template <class T>  
class Array3D  
{  
protected:  
    void alloc();  
    void free();  
public:  
    int nr;  
    int nc;  
    int nd;  
    T ***m;  
    int err;  
    Array3D(){m=0;}  
    Array3D(int r,int c,int d){nr=r;nc=c;nd=d;alloc();}  
    void Initialize(int row, int col, int dep);  
    void Finish(){free();}  
    Array3D<T> operator=(Array3D<T>& t);  
    T& operator()(int ndx, int ndy, int ndz){return m[ndx][ndy][ndz];}  
    ~Array3D(){}  
};  

//------------------------------------------  
// 2-dimensional dynamic memory allocation  
// template class : Array2D  
// 1996.10.3 by CHEN Ching-Han  
//------------------------------------------  

template <class T>  
class Array2D  
{  
protected:  
    void alloc();  
    void free();  
public:  
    int nr;  
    int nc;  
    T **m;  
    Array2D(){m=0;}  
    Array2D(int row, int col){nr=row;nc=col;alloc();}  
    void Initialize(int row, int col);  
    void Finish(){free();}  
    Array2D<T> operator=(Array2D<T>& m);  
    T& operator()(int ndx, int ndy){return m[ndx][ndy];}  
    ~Array2D(){}  
};  

//------------------------------------------  
// 1-dimensional dynamic memory allocation  
// template class : Array1D  
// 1996.10.3 by CHEN Ching-Han  
//------------------------------------------  

template <class T>  
class Array1D  
{  
protected:  
    void alloc();  
    void free();  
public:  
    int nb;  
    T *m;  
    Array1D(){m=0;}  
    Array1D(int n){nb=n;alloc();}  
    void Initialize(int n);  
    void Finish(){free();}  
    Array1D<T>& operator=(Array1D<T>& m);  
    T& operator[](int ndx){return m[ndx];}  
    ~Array1D(){}  
};  



template <class T>  
class FlexArray  
{  
private:  
    int ALLOC_INC;  
    int curSize, allocated;  
    int maxSize;  
    T **items;  
    void resize(int);  
    int MAX(int x, int y){if(x>y)return x;else return y;}  
public:  
    FlexArray(void);  
    FlexArray(int initial);  
    ~FlexArray(void);  
    T& operator[](int);  
    FlexArray<T> operator = (FlexArray<T> &);  
    int size(void) {return curSize;};  
    void setMaximumSize(int);  
    void setIncrement(int d){ALLOC_INC = d;}  
    int getMaximumSize(void) {return maxSize;};  
    void reset(void);  
    void remove(int);  
    void swap(int,int);  
};  

//  
//----- Definition of the member functions---------  
//  

template <class T>  
void Array4D<T>::alloc()  
{  
    free();  
    m=new T***[nr];  
    for(int i=0;i<nr;i++)  
    {  
        m[i]=new T**[nc];  
        for(int j=0;j<nc;j++)  
        {  
            m[i][j]=new T*[nd];  
            for(int k=0;k<nd;k++)m[i][j][k]=new T[nw];  
        }  
    }  
}  

template <class T>  
void Array4D<T>::Initialize(int row, int col, int dep, int wid)  
{  
    free();  
    nr=row;  
    nc=col;  
    nd=dep;  
    nw=wid;  
    alloc();  
}  

template <class T>  
void Array4D<T>::free( )  
{  
    if(m){  
        for(int i=0;i<nr;i++)  
        {  
            for(int j=0;j<nc;j++)  
            {  
                for(int k=0;k<nd;k++)delete m[i][j][k];  
                delete m[i][j];  
            }  
            delete m[i];  
        }  
        delete m;  
    }  
    m=0;  
}  

template <class T>  
Array4D<T> Array4D<T>::operator=(Array4D<T>& t)  
{  
    nr=t.nr;  
    nc=t.nc;  
    nd=t.nd;  
    nw=t.nw;  
    m=t.m;  
    return *this;  
}  

template <class T>  
void Array3D<T>::alloc()  
{  
    free();  
    m=new T**[nr];  
    for(int i=0;i<nr;i++)  
    {  
        m[i]=new T*[nc];  
        for(int j=0;j<nc;j++)m[i][j]=new T[nd];  
    }  
}  

template <class T>  
void Array3D<T>::Initialize(int row, int col, int dep)  
{  
    if(m!=0)free();  
    nr=row;  
    nc=col;  
    nd=dep;  
    alloc();  
}  

template <class T>  
void Array3D<T>::free( )  
{  
    if (m){  
        for(int i=0;i<nr;i++)  
        {  
            for(int j=0;j<nc;j++)delete m[i][j];  
            delete m[i];  
        }  
        delete m;  
    }  
    m=0;  
}  

template <class T>  
Array3D<T> Array3D<T>::operator = (Array3D<T>& t)  
{  
    nr=t.nr;  
    nc=t.nc;  
    nd=t.nd;  
    m=t.m;  
    return *this;  
}  

//  
//  Array2D  
//  
template <class T>  
void Array2D<T>::alloc()  
{  
    free();  
    m=new T*[nr];  
    for(int i=0;i<nr;i++)m[i]=new T[nc];  
}  

template <class T>  
void Array2D<T>::free( )  
{  
    if(m==0)return;  
    for(int i=0;i<nr;i++)delete m[i];  
    delete m;  
    m=0;  
}  

template <class T>  
void Array2D<T>::Initialize(int row, int col)  
{  
    if(m!=0)free();  
    nr=row;  
    nc=col;  
    alloc();  
}  

template <class T>  
Array2D<T> Array2D<T>::operator = (Array2D<T>& d)  
{  
    nr=d.nr;  
    nc=d.nc;  
    m=d.m;  
    return *this;  
}  

//  
//  Array1D  
//  
template <class T>  
void Array1D<T>::alloc()  
{  
    free();  
    m=new T[nb];  
}  

template <class T>  
void Array1D<T>::free( )  
{  
    if(m) delete m;  
    m=0;  
}  

template <class T>  
void Array1D<T>::Initialize(int n)  
{  
    if(m!=0)free();  
    nb=n;  
    alloc();  
}  

template <class T>  
Array1D<T>& Array1D<T>::operator=(Array1D<T>& d)  
{  
    nb=d.nb;  
    m=d.m;  
    return *this;  
}  

template <class T>  
FlexArray<T>::FlexArray(void)  
{  
    ALLOC_INC = 5;  
    curSize = 0;  
    allocated = 0;  
    maxSize = 100;  
    items = 0;  
};  


//  
// Flexible 1-D Array  
//  

template <class T>  
FlexArray<T>::FlexArray(int initial)  
{  
    curSize = 0;  
    allocated = 0;  
    maxSize = initial + 100;  
    items = 0;  
    (*this)[initial-1];  
}  

template <class T>  
FlexArray<T>::~FlexArray(void)  
{  
    for(int i=0; i < allocated; i++)	delete items[i];  
    if(items)delete items;  
};  

template <class T>  
T& FlexArray<T>::operator[](int index)  
{  
    resize(index + 1);  
    curSize = MAX(curSize, index + 1);  
    return *(items[index]);  
};  

template <class T>  
FlexArray<T> FlexArray<T>::operator = (FlexArray<T>& lSrc)  
{  
    int i;  
    resize(lSrc.size());  
    curSize = lSrc.size();  
    maxSize = lSrc.getMaximumSize();  
    for (i=0; i<curSize; i++)*(items[i]) = lSrc[i];  
    return *this;  
};  


template <class T>  
void FlexArray<T>::resize(int new_size)  
{  
    if(new_size > allocated)  
    {  
        int old_size = allocated;  
        allocated = MAX(allocated + ALLOC_INC, new_size);  
        T **new_items = new T * [allocated];  
        for(int i=0; i < old_size; i++) new_items[i] = items[i];  
        for(int i=old_size; i < allocated; i++)	new_items[i] = new T;  
        if(items)delete items;  
        items = new_items;  
    }  
    return;  
}  



template <class T>  
void FlexArray<T>::setMaximumSize(int newSize)  
{  
    maxSize = newSize;  
};  


template <class T>  
void FlexArray<T>::reset(void)  
{  
    for (int i=0; i<allocated; i++) delete items[i];  
    curSize = 0;  
    allocated = 0;  
    items = 0;  
};  


template <class T>  
void FlexArray<T>::remove(int index)  
{  
    if(index < allocated) delete items[index];  
    swap(index, allocated-1);  
    allocated--;  
    curSize--;  
    if(curSize == 0) reset();  
    return;  
};  


template <class T>  
void FlexArray<T>::swap(int index1,int index2)  
{  
    T *tempitem;  
    if (index1>=maxSize)  
    {  
        //		printf("***Requested table index = %d, table size = %d***",	index1,maxSize);  
        //		exit(1);  
    }  
    if (index2>=maxSize)  
    {  
        //		printf("***Requested table index = %d, table size = %d***",	index2,maxSize);  
        //		exit(1);  
    }  
    tempitem = items[index1];  
    items[index1] = items[index2];  
    items[index2] = tempitem;  
    curSize = MAX(index1+1,curSize);  
    curSize = MAX(index2+1,curSize);  
    return;  
};  

typedef Array3D <float> f3D;  
typedef Array2D <float> f2D;  
typedef Array1D <float> f1D;  
typedef Array2D<unsigned char> uc2D;  
typedef Array1D<unsigned char> uc1D;  
typedef Array2D <int> i2D;  
typedef Array1D <int> i1D;  
typedef Array2D <bool> b2D;  
typedef Array1D <bool> b1D;  

#endif  