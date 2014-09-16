#include "lexertl/debug.hpp"
#include "lexertl/generator.hpp"
#include <iostream>
#include "lexertl/lookup.hpp"
#include <regexParse.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dfa_graph_t *Regex_Parse(struct app_entry_list *apps)
{
	lexertl::rules rules;
    lexertl::state_machine sm;
	struct app_entry_t *app;

	STAILQ_FOREACH(app, apps, next) {
		rules.push(app->regex, app->appId);
	}

	lexertl::generator::build(rules, sm);
	//sm.minimise();
	//lexertl::debug::dump(sm, std::cout);
	return lexertl::generator::reassemble(sm);
}

void Regex_Test(struct app_entry_list *apps, char *Text)
{
	lexertl::rules rules;
    lexertl::state_machine sm;
	struct app_entry_t *app;

	STAILQ_FOREACH(app, apps, next) {
		rules.push(app->regex, app->appId);
	}

	lexertl::generator::build(rules, sm);
	sm.minimise();
	lexertl::debug::dump(sm, std::cout);

    std::string input(Text);
    lexertl::smatch results(input.begin(), input.end());

    // Read ahead
    lexertl::lookup(sm, results);

    while (results.id != 0)
    {
        std::cout << "Id: " << results.id << ", Token: '" <<
            results.str () << "'\n";
        lexertl::lookup(sm, results);
    }

	return;
}

#ifdef __cplusplus
}
#endif
