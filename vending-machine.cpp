#include <iostream>
#include <cstdarg>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <cmath>

/**
** Function to be used in comparisons of double values to eliminate
** accuracy errors that would fall below a rough approximation of epsilon
** see https://0.30000000000000004.com/
*/
bool doubleEquals( double firstVal, double secondVal ) {
    return ( std::fabs( firstVal - secondVal ) < 0.0000000001 );
}

/**
** Struct to represent one type of beverage inside the vending machine slot.
** It only contains of one simple identifier as of now which is the name
** and is implemented as structure since it is rather trivial
*/
class Drink {
    std::string name;
    // -------------------------
    // Constructors
    // -------------------------
    Drink() = default;
    Drink( const char* name ) : name( std::string( name ) ) {}
    Drink( const Drink& other ) : name( other.name ) {}
    std::string toString() { return this->name; }
    inline bool operator==( const std::string& right ) { return ( this->name == right ); }
};

/**
 * Comparison of beverage with string to identify
 * the beverage without having to access attributes
 * @expression Drink == "Coca Cola"
 */

/**
** Class to represent currency which can be used when interacting with
** the vending machine. The class is able to represent its value in a given amount
** of allowed values for coins which can be taken advantage of in exchange of currency
*/
class Price {
    /**
    ** Struct to instantiate the currency's coins which can be used to
    ** express a given set of values resulting from the combinations
    ** of coins
    */
    struct Coin {
        int amount;
        double coinValue;
        // -------------------------
        // Constructors
        // -------------------------
        Coin() = default;
        Coin(const Coin& other) {
            this->coinValue = other.coinValue;
            this->amount = other.amount;
        }
        Coin( double& coinValue, int amount ) {
            this->coinValue = coinValue;
            this->amount = amount;
        }
    };

    struct CoinArray {
        std::vector< Coin > coins; // steps of the currency plus how many are needed in order to express value
        /**
         * Function to convert internal currency value
         * expressed as coins with given categories and amounts
         * @params allowedCoins the categories or steps of the currency
         */
        CoinArray() = default;
        std::string toString() {
            std::string res;
            std::ostringstream elem;
            // iterate the evaluated coins and express them accumulated as string
            for ( std::vector< Coin >::iterator it = this->coins.begin(); it != this->coins.end(); ++it ) {
                if ( it->amount > 0 )
                    elem << '{' << std::setprecision(2) << std::fixed << it->amount << 'x' << ',' << it->coinValue <<  '}' << ',';
            }
            res.append( elem.str() );
            // delete last character to mark end of string
            res.erase( std::prev( res.end() ) );
            elem.clear();
            return res;
        }
    };

    public:
        // -------------------------
        // Constructors
        // -------------------------
        Price() : value( 0.0 ) {}
        Price( double value ) : value( value ) {}
        Price( const Price& other ) : value( other.value ) {}

        // -------------------------
        // Get and Set Functions
        // -------------------------
        void setPrice( double& value ) {
            this->value = value;
        }
        void setPrice( double&& value ) {
            this->value = value;
        }
        double getPrice() {
            return this->value;
        }

        // -------------------------
        // Operators
        // -------------------------
        // Price >= Price
        inline bool operator>=( const Price& right ){ return this->value >= right.value; }
        // Price - Price
        Price operator-( const Price& other ) {
            Price temp( this->value - other.value );
            return temp;
        }
        // Price * 3
        Price operator*( const int& other ) {
            Price temp( this->value * other );
            return temp;
        }
        // Price = Price
        Price& operator=( const Price& other ) {
            this->value = other.value;
            return (*this);
        }
        // Price = 3.00
        Price& operator=( double&& other ) {
            this->value = other;
            return (*this);
        }

        /**
         * Function to convert internal currency value
         * expressed as coins with given categories and amounts
         * @params allowedCoins the categories or steps of the currency
         */
        CoinArray asCoins( std::vector< double > allowedCoins ) {
            CoinArray res;
            // copy of value to not edit original value
            double tmp = this->value;
            for ( double &i : allowedCoins ) {
                res.coins.push_back( Coin( this->reduce( tmp, i ) ) );
            } // reduce value until zero
            if ( !doubleEquals( 0, tmp ) )
                res.coins.clear();
            // no result when zero cannot be reached
            return res;
        }

        /**
         * Function to express internal currency value
         * as string for generating output on the console
         */
        std::string toString() {
            std::ostringstream elem;
            elem << std::setprecision(2) << std::fixed << this->value;
            return elem.str();
        }

    private:
        // -------------------------
        // Values
        // -------------------------
        double value; // as floating point number

        /**
         * Internal function to reduce the internal saved value
         * of a given category or step. It works similar to mod
         * except that floating points are allowed here.
         * @params val the currency value
         * @params cat currency category
         */
        Coin reduce( double& val, double cat ) {
            int amount = 0;
            if ( doubleEquals( val, cat ) ) {
                Coin coin( cat, 1 );
                val = 0.0;
                return coin;
            }
            while ( val > cat ) {
                val -= cat;
                amount++;
            }
            // store category with amount
            Coin coin( cat, amount );
            return coin;
        };
};

/**
** Class to represent the vending machine containing slots to fill with limited beverages
** of a non-const type as well as providing an accesspoint for purchasing which is also
** customizable with any currency type to provide basic functionality
*/
template< typename DrinkType, typename PriceType, typename CountType = unsigned int >
class VendingMachine {
    /**
    ** Struct to instantiate a single slot inside the vending machine
    ** for which a set of three different values are needed
    ** that contain all information about the slot
    */
    struct Slot {
        DrinkType drink;
        PriceType price; // what one drink costs
        CountType amount; // how many drinks
        // -------------------------
        // Constructors
        // -------------------------
        Slot() = default;
        Slot( DrinkType drink, PriceType price, CountType amount ) : drink(drink), price(price), amount(amount) {}
        Slot( const Slot& other ) : drink(other.drink), price(other.price), amount(other.amount) {}
    };

    public:
        // -------------------------
        // Constructors
        // -------------------------
        VendingMachine() = default;

        /**
         * Fill the vending machine with its
         * necessary items for each slot
         * @params variadic as well as vectors see main()
         */
        void fill( DrinkType drink, PriceType price, CountType amount ) {
            Slot temp( drink, price, amount );
            this->drinks.push_back( temp );
        }
        template< typename ... Args >
        void fill( DrinkType drink, PriceType price, CountType amount, Args ... args ) {
            Slot temp( drink, price, amount );
            this->drinks.push_back( temp );
            this->fill( args... );
        }
        void fill( std::vector< DrinkType > drinks, std::vector< PriceType > prices, std::vector< CountType > amounts ) {
            typename std::vector< DrinkType >::const_iterator dIt = drinks.begin();
            typename std::vector< PriceType >::const_iterator pIt = prices.begin();
            typename std::vector< CountType >::const_iterator aIt = amounts.begin();
            // iterate all vectors at once and join values to one slot each iteration
            for ( ;dIt != drinks.end() && pIt != prices.end() && aIt != amounts.end();
                ++dIt, ++pIt, ++aIt ) {
                    Slot temp( (*dIt), (*pIt), (*aIt) );
                    this->drinks.push_back( temp );
            }
        }

        /**
         * Buy one type of drink from the vending machine a given amount of times
         * based on a selector to identify the drink (requires an implementation of ==)
         * @params selector to match drink
         * @params amount how many times to purchase
         * @params change the money from the customer
         */
        template< typename T >
        DrinkType buy( T selector, CountType amount, PriceType& change ) {
            typename std::vector< Slot >::iterator it = this->drinks.begin();
            DrinkType* drink = nullptr;
            PriceType* cost = nullptr;
            // iterate drinks to find the selected one
            for ( ;it != this->drinks.end(); ++it ) {
                if ( it->drink == selector ) {
                    drink = &it->drink;
                    cost = &it->price;
                    if ( (amount <= it->amount) && (change >= (it->price * (int)amount)) ) {
                        it->amount -= amount;
                        break;
                    }
                }
            }
            // if the selection was found in the vending machine
            if ( it != this->drinks.end() ) {
                for ( CountType i = 0; i < amount; i++ )
                    change = change - (*cost);
                DrinkType temp( *drink );
                return temp; // return found object
            }
            Drink temp;
            return temp; // return default constructed object
        }

        /**
         * Vending machine contents as string representation
         * for outputing on the console
         */
        std::string toString() {
            std::string res;
            std::ostringstream elem;
            // iterate all available drinks
            typename std::vector< Slot >::iterator it = this->drinks.begin();
            for( ;it != this->drinks.end(); ++it ) {
                elem << std::setprecision(2) << std::fixed << '{' << it->amount << 'x' << ',' << it->drink.toString() << ',' << it->price.toString() << '$' << '}' << ',';
            }
            res.append(elem.str());
            res.erase(std::prev(res.end()));
            elem.clear();
            return res;
        }

        // -------------------------
        // Values
        // -------------------------
    private:
        std::vector< Slot > drinks; // all slots with its drinks
};

// type to quantify the occurrence of beverages
typedef unsigned int Count;

int main() {
    // set vending machine and its available currency categories
    VendingMachine< Drink, Price > vending_machine;
    std::vector< double > coins = { 2.0, 1.0, 0.5, 0.2, 0.1 };

    // example of filling the machine from vectors
    std::vector< Drink > drinks { "Coca Cola", "Grape Soda" };
    std::vector< Price > prices { 2.00, 1.50 };
    std::vector< Count > amounts { 3, 5 };
    vending_machine.fill( drinks, prices, amounts );

    // example of filling the machine from arbitrary parameters
    Price A( 1.3 );
    Drink B("Bottled Water");
    vending_machine.fill( Drink("Orange Soda"), A, 6,
            B, Price(1.2), 8,
            Drink("Sapphire Martini"), 13.0, 1);

    // customer selection from the machine which equals 2x Coca Cola, 2x Orange Soda and 1x Bottled Water
    std::string selectedDrinks[] = { {"Coca Cola"}, {"Orange Soda"}, {"Bottled Water"} };
    unsigned int selectedAmounts[] = { {2}, {2}, {1} };
    // customers available currency
    Price change(12.60);

    std::cout << "Vending Machine" << ':' << ' ' << vending_machine.toString() << std::endl;
    std::cout << "Available Currency" << ':' << ' ' << change.toString() << '$' << ' ' << "in coins" << ' ' << change.asCoins(coins).toString() << std::endl;
    std::cout << std::endl;

    // making customer purchases with currency
    for ( std::size_t i = 0; i < std::size( selectedDrinks ); i++ ) {
        Drink bought = vending_machine.buy< std::string >( selectedDrinks[i], selectedAmounts[i], change );
        std::cout << "Drink which was bought" << ':' << ' ' << selectedAmounts[i] << "x" << ' ' << bought.name << std::endl;
    }
    std::cout << std::endl;

    // making an invalid purchase which fails silently
    vending_machine.buy< std::string >( "Sapphire Martini", 1, change );

    std::cout << "Remaining in Machine" << ':' << ' ' << vending_machine.toString() << std::endl;
    std::cout << "Remaining Currency" << ':' << ' ' << change.toString() << '$' << ' ' << "in coins" << ' ' << change.asCoins(coins).toString() << std::endl;
    std::cout << std::endl;

    return 0;
}
