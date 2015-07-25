
class MySwitch {
	String name;
	unsigned long on_code;
	unsigned long off_code;
	int bitlength;
	RCSwitch rcSwitch;


	//{name, on_code, off_code, bitlength}
	public:

		//Constructor
		MySwitch();
		MySwitch(String _name, unsigned long _on_code, unsigned long _off_code, int _bitlength);
		MySwitch(String _name, unsigned long _on_code, unsigned long _off_code, int _bitlength, RCSwitch *_rcSwitch);
		MySwitch(String *_name, unsigned long *_on_code, unsigned long *_off_code, int *_bitlength, RCSwitch *_rcSwitch);

		//Functions
		void transmit24bit(RCSwitch *rcSwitch);
		void transmit24bit();
		void on();
		void off();
		void transmit24(unsigned long code);
		void send(bool state);

		void printSwitch();
		bool match(unsigned long value);
		bool matchSignal(unsigned long value);



		//Getters
		String getName();
		unsigned long getOnCode();
		unsigned long getOffCode();
		int getBitlength();

	protected:

};

