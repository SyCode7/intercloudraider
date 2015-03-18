## Background ##

Cloud based services have become an integral part for data storage and backup solutions used by many organizations as well as individuals. While the main commercial players generally provide robust and reliable service, guided both by the need to maintain good reputation as well as legal obligations, our work explores an approach which enables the end-users to achieve better reliability and confidentiality while outsourcing their storage without being left to the mercy of the goodwill of any single service provider.

This is important because individual storage service providers may be compromised
by hardware or software faults, a specific service provider may fall prey to hacking attack or a service may be compromised by malicious employees. Not to mention the issue of overzealous government agencies using or abusing legal instruments to access private data. Likewise, when an user deletes his data from a service provider, there is no guarantee that all the copies that the service provider may have created (for fault-tolerance) are actually removed. Consequently, the exposure of data may persist beyond the period an user uses a specific service.

Storing only encrypted data can arguably deal with the confidentiality issue. However it does not prevent the problem of data loss. Creating a service which spreads the data across multiple clouds is a logical way to achieve this. In order to keep the overhead
of redundancy low, using erasure coding is a natural choice that is deployed by all such systems. Our work leverages the same broad ideas, but unlike these existing works, which demonstrate the concepts using services like Amazon S3, RackSpace, etc. that are aimed to cater to the businesses, we build a working system (InterCloud RAIDer) using more ‘plebeian’ services such as Dropbox, Google Drive, SkyDrive, etc. which also cater to individual users, particularly by providing a stripped down service for free, which can then be augmented with more resources if needed, by paying. In doing so, we hope that InterCloud RAIDer can be readily utilized by individuals for personal use.


## InterCloud RAIDer ##

InterCloud RAIDer is a multi-cloud private data backup system for outsourcing data to untrusted cloud storage services. Its core features including:

  * A data deduplication technique to reduce the overall storage overhead.

  * An erasure coding method to achieve redundancy at low overhead, where the encoded blocks are dispersed across multiple cloud services to realize fault-tolerance against individual service providers, specifically we use non-systematic instances of erasure codes to provide a basic level of privacy from individual cloud stores.

  * A proof of data possession mechanism to detect misbehaving services - where we optimize the implementation by exploiting hash digests that are created in the prior deduplication phase.

Our paper containing the detailed description of InterCloud RAIDer can be found at [here](http://sands.sce.ntu.edu.sg/CodingForNetworkedStorage/pdf/intercloudraider.pdf). In the current implementation, the system is supporting Dropbox, Google Drive, SkyDrive and Ubuntu One.

## Using InterCloud RAIDer ##
The instructions for how to install InterCloud RAIDer can be found in the [Installation](Installation.md) while the instructions for how to use InterCloud RAIDer can be found in the [ManualPage](ManualPage.md).