'''
    This script sends a query to Twitter. It requires a subscription to a Twitter developer account. The Twitter credentials to access this account must be saved in a yaml file called: "~/.twitter_keys.yaml". The returned Tweets are stored in a raw text file: '../../Twitter_cred/full_arch_2007-2020.txt'. 
        !!!WARNING!!!
        Do not store the twitter credentials or the returned Tweets in this GitHub repository, since it can be accessed by anyone.
'''


from searchtweets import ResultStream, gen_rule_payload, load_credentials,collect_results
import json

def searchtweets_query(file_name='../../Twitter_cred/full_arch_2007-2020.txt'):

    premium_search_args = load_credentials(filename="~/.twitter_keys.yaml",
                     yaml_key="search_tweets_fullarchive_dev",
                     env_overwrite=False)

    print(premium_search_args)
    print()

    #query = "(nat OR natte OR water OR wateroverlast OR regen OR storm OR blank OR bui OR overstroming OR hoosbui OR schade OR noodweer OR wolkbreuk OR waterschade) has:geo place_country:NL"
    query = "(wateroverlast OR overstroming OR waterschade) has:geo place_country:NL"

    do_query = False

    if do_query:
        from_date = "2007-01-01"
        to_date = "2020-01-01"
        rule = gen_rule_payload(query, results_per_call=500,from_date=from_date,to_date=to_date)
        tweets = collect_results(rule,
                                     max_results=500*50,
                                     result_stream_args=premium_search_args) # change this if you need to
        for tweet in tweets:
            with open(file_name, 'a') as fp:
                fp.write(json.dumps(tweet)+'\n')
    else:
        print("No query was done, in order to perform a Twitter query, set do_query to True in Twitter/searchtweets_query.py")

if __name__ == '__main__':
    searchtweets_query()
