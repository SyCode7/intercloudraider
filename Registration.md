## Key and Secret ##

The users have to register for an account at the cloud service providers that are supporting by InterCloud RAIDer and get a pair of **key** and **secret** after registration. During the [first running of InterCloud RAIDer](ManualPage#First_Running_of_InterCloud_RAIDer.md), the system will request for the pair of key and secret that is retrieving from the service providers. The system will then use the pair of credentials for [authorization](ManualPage#Authorization_Protocol.md) when making access requests to the service providers.

The registration procedures of different cloud service providers are describing as follows:

### Dropbox ###

If you don't have a Dropbox account, please log on to  https://www.dropbox.com/register and register for an account by providing an valid email.

Then,

  * Please go to https://www.dropbox.com/developers/apps/create and create an application for InterCloud RAIDer.

  * After creating an application, the webpage will be redirect to application's credentials page.

  * Take note of the credential pair of **app key** and **app secret** as you'll need to input it for the first running of InterCloud RAIDer.

If you have already created an application for InterCloud RAIDer, you can always retrieve the credential pair at [Dropbox App Console](https://www.dropbox.com/developers/apps).

### Google Drive ###

If you don't have a Google account, please log on to  https://accounts.google.com/SignUp and register for an account.

Then,
  * If you haven't already registered your application with the Google Cloud Console, then [set up a project and application in the Cloud Console](https://cloud.google.com/console#/flows/enableapi?apiid=drive) for InterCloud RAIDer. The system guides you through the process of choosing or creating a project and registering a new application, and it automatically activates the API for you.

  * If you've already registered your application with the Cloud Console, then follow this procedure instead:
    1. Go to [Google Cloud Console](https://cloud.google.com/console#/project).
    1. Select a project.
    1. In the sidebar on the left, select **APIs & auth**. In the displayed list of APIs, make sure the Drive API status is set to **ON**.
    1. In the sidebar on the left, select **Registered apps**.
    1. Select an application.

  * In either case, you end up on the application's credentials page.

  * To find your application's client ID and client secret, and set a redirect URI (please set to _urn:ietf:wg:oauth:2.0:oob_), expand the OAuth 2.0 Client ID section.

  * Take note of the pair of **Client ID** and **Client secret** as you'll need to input it for the first running of InterCloud RAIDer.

The details of the instructions can be found at [Google Drive SDK](https://developers.google.com/drive).

### SkyDrive ###

If you don't have a Microsoft account, please log on to  https://signup.live.com/ and register for an account.

Then,
  * Please go to [My apps](https://account.live.com/developers/applications/create) at Live Connect Developer Center and create an application for InterCloud RAIDer.

  * After creating an application, the webpage will be redirect to application's credentials page.

  * Take note of the credential pair of **Client ID** and **Client secret** as you'll need to input it for the first running of InterCloud RAIDer.

If you have already created an application for InterCloud RAIDer, you can always retrieve the credential pair at [My applications](https://account.live.com/developers/applications) after signing in your account.

### Ubuntu One ###

If you don't have an Ubuntu One account, please log on to https://login.ubuntu.com/+login and register for an account by providing an valid email.

The **consumer key** and **consumer secret** are automatically set to _ubuntuone_ and _hammertime_.