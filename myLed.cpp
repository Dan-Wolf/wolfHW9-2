#include <iostream>         // for the input/output
#include <stdlib.h>         // for the getenv call
#include <sys/sysinfo.h>    // for the system uptime call
#include <cgicc/Cgicc.h>    // the cgicc headers
#include <cgicc/CgiDefs.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>
#include <unistd.h>
#define LED_GPIO "/sys/class/gpio/gpio60/"
using namespace std;
using namespace cgicc;

void writeGPIO(string filename, string value){
   fstream fs;
   string path(LED_GPIO);
   fs.open((path + filename).c_str(), fstream::out);
   fs << value;
   fs.close();
}

int main(){
   Cgicc form;                      // the CGI form object
   string cmd;                      // the Set LED command
   string num = "-1";
   writeGPIO("direction", "out");
   // get the state of the form that was submitted - script calls itself
   bool isStatus = form.queryCheckbox("status");
   form_iterator it = form.getElement("cmd");  // the radio command
   if (it == form.getElements().end() || it->getValue()==""){
      cmd = "off";                     // if it is invalid use "off"
   }
   else { cmd = it->getValue(); }      // otherwise use submitted value

   form_iterator i = form.getElement("num");
   if (i == form.getElements().end() || it->getValue() == "") {
	   num = "0";
   }
   else { num = i->getValue();}
   char *value = getenv("REMOTE_ADDR");    // The remote IP address
 
   // generate the form but use states that are set in the submitted form
   cout << HTTPHTMLHeader() << endl;       // Generate the HTML form
   cout << html() << head() << title("CPE 422/522 Project: Post LED Example") << head() << endl;
   cout << "<body style=\"background-color:violet;\">" << "<h1 style=\"color:white;\"> CPE 422/522 Project: Post LED Example </h1>" <<h1("Controlling a GPIO pin via a web browser") << endl;
   cout << "<form action=\"/cgi-bin/myLed.cgi\" method=\"POST\">\n";
   cout << "<div>Set LED: <input type=\"radio\" name=\"cmd\" value=\"on\""
        << ( cmd=="on" ? "checked":"") << "/> On ";
   cout << "<input type=\"radio\" name=\"cmd\" value=\"off\""
        << ( cmd=="off" ? "checked":"") << "/> Off ";
   cout << "<input type=\"radio\" name=\"cmd\" value=\"blink\""
	<< ( cmd=="blink" ? "checked":"") << "/> Blink";
   cout << "<input type=\"submit\" style=\"background-color:green;\" value=\"Set LED\" />";
   cout << "</div> <br>";
   cout << "<div>Number of Blinks: <input type=\"text\" name=\"num\" />";
   cout << "</div></form></body>";

 
   // process the form data to change the LED state
   if (cmd=="on") writeGPIO("value", "1");              // turn on
   else if (cmd=="off") writeGPIO("value", "0");        // turn off
   else if (cmd=="blink") {
	   if (num == "") { cout << "<div> Enter number for blink command </div>" << endl; }
	   else {
		cout << "<div> Blinking " << num << " times </div>";
	   	int count = stoi(num);
	   	for (int j=0; j<count; j++){
			writeGPIO("value", "1");
			sleep(1);
		   	writeGPIO("value", "0");
		   	sleep(1);
	   	}
	   }

   }
   else cout << "<div> Invalid command! </div>";        // not possible
   cout << body() << html();
   return 0;
}
