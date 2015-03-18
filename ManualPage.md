## First Running of InterCloud RAIDer ##
After installation of the InterCloud RAIDer, the users will be requested for a pair of [key and secret](Registration#Key_and_Secret.md) during the first running which is needed for [authorization](ManualPage#Authorization_Protocol.md) when making access requests to cloud services according to [OAuth1.0](http://oauth.net/core/1.0/)/[OAuth2.0](http://oauth.net/). The pair of credentials will then be stored in the local database of InterCloud RAIDer. During the authorization process, the system will retrieve the pair from the local database.

The users can generate the key and secret pair by following the instructions in the [Registration](Registration.md).

### Modify Key and Secret ###
The users may check and modify the key and secret even after first input:
  1. Click **Settings** in the side bar on the top left.
  1. Click **Background Parameter** and proceed to Background Parameter Page.


## Functions of InterCloud RAIDer ##
The InterCloud RAIDer provides the following functions: **Backup**,
**Retrieve**, **Delete**, and **Verify**.

The following subsections describe how to manually operate those functions by using GUI of InterCloud RAIDer.

### Backup ###
The users can use the backup function of InterCloud RAIDer to store files across multiple cloud service providers to realize fault-tolerance against individual service providers with minimum redundancy.

To begin, please follow the following steps:
  1. Click **Start New Operation**.
  1. Select **Back up** in the new pop-up window and click **OK**.
  1. Choose a file/folder by browsing the local file system.
  1. Click **OK** and proceed to Summary Page.
  1. Click **Run** to execute.

### Retrieve ###
The users can use the retrieve function of InterCloud RAIDer to retrieve the files being stored at cloud service providers.

To begin, please follow the following steps:
  1. Click **Start New Operation**.
  1. Select **Retrieve** in the new pop-up window and click **OK**.
  1. Choose an entry from the operation log and choose a path by browsing the local file system to save the retrieving file.
  1. Click **OK** and proceed to Summary Page.
  1. Click **Run** to execute.

### Delete ###
The users may delete the unwanted, outdated or redundant copies of files from the database of InterCloud RAIDer.

To begin, please follow the following steps:
  1. Click **File** in the side bar on the top left.
  1. Click **View Operation Log**.
  1. Select an entry to be deleted.
  1. Click **Delete** to execute.

### Verify ###
The users may verify the integrity of outsourced data at every service provider, so that the data is not compromised by misbehaving of individual services, e.g. illegitimate modification or deletion, by using Provable Data Possession (PDP) mechanism. We omit the details of PDP and the users who are interested in PDP can refer to [InterCloud RAIDer](http://sands.sce.ntu.edu.sg/CodingForNetworkedStorage/pdf/intercloudraider.pdf).

To begin, please follow the following steps:
  1. Click **Tools** in the side bar on the top left.
  1. Click **PDP Verification** and click **OK** in the pop-up window.
  1. Select the service you wish to verify for the integrity and click **OK** to execute.

## Authorization Protocol ##
For every running of InterCloud RAIDer, the users will be asked for the consent to process the authorization protocol of [OAuth1.0](http://oauth.net/core/1.0/)/[OAuth2.0](http://oauth.net/) when making access requests to cloud services during the execution of the functions, e.g., [backup](ManualPage#Functions_of_InterCloud_RAIDer.md), [retrieve](ManualPage#Functions_of_InterCloud_RAIDer.md), [delete](ManualPage#Functions_of_InterCloud_RAIDer.md), and [verify](ManualPage#Functions_of_InterCloud_RAIDer.md).

The users are either be requested to approve the authorization process by clicking "**Allow**", "**Accept**" or "**Add this computer**" by visiting the webpages of the cloud services, or be requested to provide an **one-time verification code** which can be retrieved during the process.

The steps to formalize the authorization process of each cloud service are described as follows:

### Dropbox ###
  1. Click **Open Link** in the pop-up window entitled **Dropbox Authorization**.
  1. Click **Allow** in the pop-up webpage.
  1. Click **OK** in the pop-up window entitled **Proceed to next step**.

### Google Drive ###
  1. Click **Open Link** in the pop-up window entitled **Google Drive  Authorization**.
  1. Click **Accept** in the pop-up webpage.
  1. Copy the **verification code** from the webpage and paste it in the line edit of the pop-up window entitled **Proceed to next step**. The verification code is in the bar under the text "_Please copy this code, switch to your application and paste it there:_" of the webpage.

### SkyDrive ###
  1. Click **Open Link** in the pop-up window entitled **SkyDrive  Authorization**.
  1. Sign in your SkyDrive account with valid email.
  1. Copy the **verification code** from the redirected webpage and paste it in the line edit of the pop-up window entitled **Proceed to next step**. The verification code is located in the **url bar** of the webpage, after the text **code=**.

### Ubuntu One ###
  1. Click **Open Link** in the pop-up window entitled **Ubuntu One  Authorization**.
  1. Click **Add This Computer** in the pop-up webpage.
  1. Copy the **verification code** from the redirected webpage and paste it in the line edit of the pop-up window entitled **Proceed to next step**. The verification code is located in the **url bar** of the webpage, between the text **oauth\_verifier=** and **&return=**.