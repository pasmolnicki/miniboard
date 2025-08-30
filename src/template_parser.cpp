#include "template_parser.h"

#define log_m(x) do { } while(0)
#define logf_m(fmt, ...) do { } while(0)

char peek(const char* buf, int len, int i) {
    return i + 1 < len ? buf[i + 1] : 0;
}

bool advance(int& i, int len) {
    if (i < len - 1) {
        i++;
        return true;
    }
    return false;
} 

// Template values are formatted this way: <%NAME%>
void parse(const char* temp, char* page, parser_args_t args) {
    
    int iter = 0,
        page_iter = 0,
        end = strlen(temp);

    while (iter != end) {

        // That's presumably a template argument
        if (temp[iter] == '<' && peek(temp, end, iter) == '%') {
            log_m("Found <%");

            int arg_name_iter = iter + 1;

            bool valid = false;
            while (advance(arg_name_iter, end)) {
                if (temp[arg_name_iter] == '%' && peek(temp, end, arg_name_iter) == '>') {
                    valid = true;
                    break;
                }
            }

            logf_m("Arg name ends at %d, valid=%d\n", arg_name_iter, valid);

            // This is a template argument
            if (valid) {
                std::string arg_name(temp + iter + 2, temp + arg_name_iter);
                logf_m("Looking for argument: %s\n", arg_name.c_str());

                bool found = false;
                for (auto &arg : args) {
                    if (arg.name == arg_name) {
                        // Insert the value
                        found = true;
                        for (int i = 0; i < arg.value.size(); i++, page_iter++) {
                            page[page_iter] = arg.value[i];
                        }
                        logf_m("Replaced argument %s with value %s\n", arg.name, arg.value.c_str());
                        break;
                    }
                }

                if (found) {
                    logf_m("New iter starts at %d\n", iter);
                    iter = arg_name_iter + 2;
                    continue;
                }
            }

            log_m("Not valid or didn't find the argument");
            // not valid or didn't find the argument, simply copy the text
            for (; iter < arg_name_iter; iter++, page_iter++) {
                page[page_iter] = temp[iter];
            }
            continue;
        }
        
        logf_m("cpy '%c'\n", temp[iter]);
        page[page_iter] = temp[iter];
        page_iter++;
        iter++;
    }

    page[page_iter] = 0; // null terminate

    logf_m("Final page length: %d\n", page_iter);
    logf_m("Final page content:\n%s\n", page);
}