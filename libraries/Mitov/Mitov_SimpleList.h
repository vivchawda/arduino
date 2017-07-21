// SimpleList.h

#ifndef _MITOV_SIMPLELIST_h
#define _MITOV_SIMPLELIST_h

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

#if defined( VISUINO_CONTROLLINO_MINI ) || defined( VISUINO_CONTROLLINO_MAXI ) || defined( VISUINO_CONTROLLINO_MAXI_AUTOMATION ) || defined( VISUINO_CONTROLLINO_MEGA ) // || defined( VISUINO_FREESOC2 )
	#define nullptr 0
#else
	#if __cplusplus < 201402L
		#define nullptr 0
	#endif
#endif

#if defined( VISUINO_CONTROLLINO_MINI )
	#define CONTROLLINO_MINI
#endif

#if defined( VISUINO_CONTROLLINO_MAXI )
	#define CONTROLLINO_MAXI
#endif

#if defined( VISUINO_CONTROLLINO_MEGA )
	#define CONTROLLINO_MEGA
#endif

//#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
//#else
//#include "WProgram.h"
//#endif

#ifndef NULL
#define NULL 0
#endif

namespace Mitov
{

/*
template<typename T> const T MitovConstrain(const T a, const T b, const T c )
{
	Serial.print( "a: " );
	Serial.print( a );
	Serial.print( " b: " );
	Serial.print( b );
	Serial.print( " c: " );
	Serial.println( c );
	if( a > c )
		return c;

    return (b > a) ? b : a;
}
*/

template<typename T> const T MitovMin(const T a, const T b)
{
    return (b < a) ? b : a;
}

template<typename T> const T MitovMax(const T a, const T b)
{
    return (b > a) ? b : a;
}

template<typename T, typename T_SIZE = uint8_t>
class SimpleList
{
public:
	SimpleList()
	{
	}

	SimpleList( const SimpleList &AOther )
	{
		clear();
		append( AOther, AOther.size() );
	}

    ~SimpleList()
    {
        delete[] _list;
        _list = nullptr;
        _size = 0;
    }

public:
	void AddCount( int ACount )
	{
		for( T_SIZE i = 0; i < ACount; ++i )
			push_back( T() );
	}

	void SetCount( int ACount )
	{
		while( _size < ACount )
			push_back( T() );

		while( _size > ACount )
			pop_back();
	}

	void SetCount( int ACount, T AValue )
	{
		while( _size < ACount )
			push_back( AValue );

		while( _size > ACount )
			pop_back();
	}

    void push_back(T obj)
    {
        ++_size;

//		Serial.println( "TEST1" );

        T* newBuffer = new T[_size];

        if( _list )
        {
            for ( T_SIZE i = 0; i < _size - 1; ++i)
                newBuffer[i] = _list[i];

            delete[] _list;
        }

//		Serial.println( "TEST2" );
//		delay( 100 );

        newBuffer[_size - 1] = obj;
        _list = newBuffer;
    }

    void append(T *obj, int ACount )
    {
		if( ! obj )
			return;

		if( !ACount )
			return;

		int AOldSize = _size;
        _size += ACount;

        T* newBuffer = new T[_size];
        if( _list )
        {
    		memcpy( newBuffer, _list, AOldSize * sizeof( T ));
    		delete[] _list;
        }

		memcpy( newBuffer + AOldSize, obj, ACount * sizeof( T ));

        _list = newBuffer;
    }

    void push_front(T obj)
    {
        ++_size;
        T* newBuffer = new T[_size];

        if( _list )
        {
            for ( T_SIZE i = _size - 1; i > 0; --i)
                newBuffer[i] = _list[i - 1];

    		delete[] _list;
        }

        newBuffer[0] = obj;
        _list = newBuffer;
    }

    void pop_back()
    {
        if (empty())
            return;

        --_size;
        if (empty())
        {
			delete[] _list;
            _list = nullptr;
            return;
        }

        T* newBuffer = new T[_size];
		for ( T_SIZE i = 0; i < _size; ++i)
			newBuffer[i] = _list[i];

        delete[] _list;
        _list = newBuffer;
    }

    void pop_front( int ACount = 1 )
    {
        if (empty())
            return;

		if( ACount >= _size )
		{
//			Serial.println( "CLEAR" );
			clear();
			return;
		}

//		int AOldSize = _size;
        _size -= ACount;

        T* newBuffer = new T[_size];

//		memmove( newBuffer, _list + ACount, _size * sizeof( T )); // Do not use memmove in case of objects!
        for ( T_SIZE i = 0; i < _size; ++i )
	        newBuffer[i] = _list[i + ACount];

        delete[] _list;
        _list = newBuffer;
    }

	void erase( T AItem )
	{
        if (empty())
            return;

        --_size;
        if (empty())
        {
			delete[] _list;
            _list = nullptr;

            return;
        }

        T* newBuffer = new T[_size];

        bool sum = false;
        for ( T_SIZE i = 0; i < _size; ++i)
        {
            if ( _list[ i ] == AItem )
                sum = true;

            if (sum)
                newBuffer[i] = _list[i + 1];

            else
                newBuffer[i] = _list[i];
        }

		delete[] _list;
        _list = newBuffer;
    }

	void Delete( T_SIZE AIndex )
	{
		if( AIndex >= _size )
			return;

        --_size;
        if (empty())
        {
			delete[] _list;
            _list = nullptr;

            return;
        }

        T* newBuffer = new T[_size];

        for ( T_SIZE i = 0; i < _size; ++i)
        {
            if ( i < AIndex )
                newBuffer[i] = _list[i];

            else
                newBuffer[i] = _list[i + 1];
        }

		delete[] _list;
        _list = newBuffer;
	}

	void Delete( T_SIZE AIndex, T_SIZE ASize )
	{
		if( AIndex >= _size )
			return;

		ASize = MitovMin( ASize, _size - AIndex );

		_size -= ASize;

        if (empty())
        {
			delete[] _list;
            _list = nullptr;

            return;
        }

        T* newBuffer = new T[_size];

        for ( T_SIZE i = 0; i < _size; ++i)
        {
            if ( i < AIndex )
                newBuffer[i] = _list[i];

            else
                newBuffer[i] = _list[i + ASize ];
        }

		delete[] _list;
        _list = newBuffer;
	}

    void clear()
    {
        if (_list)
        {
            delete[] _list;
            _list = nullptr;
        }

        _size = 0;
    }

	int IndexOf( T AItem )
	{
		for( int i = 0; i < _size; ++i )
			if( AItem == _list[ i ] )
				return i;

		return -1;
	}

    inline bool empty() const { return !_size; }
    inline T_SIZE size() const { return _size; }
    inline operator T*() const { return _list; }

	SimpleList<T, T_SIZE> & operator = ( const SimpleList<T, T_SIZE> &other )
	{
		assign( other );
		return *this;
	}

	void assign( const SimpleList<T, T_SIZE> &other )
	{
		if( &other == this )
			return;

//		Serial.println( "ASSIGN" );
		clear();
		append( other, other.size() );
	}

protected:
    T* _list = nullptr;
    T_SIZE _size = 0;
};

template<typename T, typename T_SIZE = uint8_t>
class SimpleObjectList : public SimpleList<T, T_SIZE>
{
	typedef SimpleList<T, T_SIZE>	inherited;

public:
    ~SimpleObjectList()
    {
		for( int i = 0; i < inherited::_size; ++i )
			delete inherited::_list[ i ];
	}
};

template<typename T, typename T_SIZE = uint8_t>
class SimpleObjectDeleteList : public SimpleList<T, T_SIZE>
{
	typedef SimpleList<T, T_SIZE>	inherited;

public:
    void pop_back()
	{
		if( inherited::_size )
			delete inherited::_list[ inherited::_size - 1 ];

		inherited::pop_back();
	}

    void pop_front( int ACount = 1 )
	{
		for( int i = 0; i < MitovMin<T_SIZE>( ACount, inherited::_size ); ++ i )
			delete inherited::_list[ i ];

		inherited::pop_front( ACount );
	}

	void erase( T AItem )
	{
		delete AItem;
		inherited::erase( AItem );
	}

	void Delete( T_SIZE AIndex )
	{
		if( AIndex < inherited::_size )
			delete inherited::_list[ AIndex ];

		inherited::Delete( AIndex );
	}

	void Delete( T_SIZE AIndex, T_SIZE ASize )
	{
		for( int i = AIndex; i < MitovMin<T_SIZE>( AIndex + ASize, inherited::_size ); ++i )
			delete inherited::_list[ i ];

		inherited::Delete( AIndex, ASize );
	}


    void clear()
	{
		for( int i = 0; i < inherited::_size; ++i )
			delete inherited::_list[ i ];

		inherited::clear();
	}

public:
    ~SimpleObjectDeleteList()
    {
		for( int i = 0; i < inherited::_size; ++i )
			delete inherited::_list[ i ];
	}
};

} // Mitov

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

#endif
