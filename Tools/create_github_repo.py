import os
from ghapi.all import GhApi

token = os.environ.get('GITHUB_TOKEN', '')
if not token:
    print('ERROR: GITHUB_TOKEN not set')
    exit(1)

api = GhApi(token=token)

try:
    user = api.users.get_authenticated()
    username = user['login']
    print(f'Authenticated as: {username}')

    try:
        repo = api.repos.create_for_authenticated_user(
            name='power-games-simulator',
            description='A deep political strategy game centered on power struggle - Power Game: The Art of Political Intrigue',
            homepage='',
            private=False,
            has_issues=True,
            has_projects=True,
            has_wiki=True,
            auto_init=False
        )
        print(f'Repository created: {repo["html_url"]}')
        print(f'Clone URL: {repo["clone_url"]}')
    except Exception as e:
        if 'already exists' in str(e):
            print(f'Repository already exists: https://github.com/{username}/power-games-simulator')
        else:
            raise

    print(f'USERNAME={username}')

except Exception as e:
    print(f'Error: {e}')
    exit(1)
