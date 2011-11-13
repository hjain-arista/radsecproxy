/*
 * The person or persons who have associated work with this document
 * (the "Dedicator" or "Certifier") hereby either (a) certifies that,
 * to the best of his knowledge, the work of authorship identified is
 * in the public domain of the country from which the work is
 * published, or (b) hereby dedicates whatever copyright the
 * dedicators holds in the work of authorship identified below (the
 * "Work") to the public domain. A certifier, moreover, dedicates any
 * copyright interest he may have in the associated work, and for
 * these purposes, is described as a "dedicator" below.
 *
 * A certifier has taken reasonable steps to verify the copyright
 * status of this work. Certifier recognizes that his good faith
 * efforts may not shield him from liability if in fact the work
 * certified is not in the public domain.
 *
 * Dedicator makes this dedication for the benefit of the public at
 * large and to the detriment of the Dedicator's heirs and
 * successors. Dedicator intends this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights under
 * copyright law, whether vested or contingent, in the Work. Dedicator
 * understands that such relinquishment of all rights includes the
 * relinquishment of all rights to enforce (by lawsuit or otherwise)
 * those copyrights in the Work.
 *
 * Dedicator recognizes that, once placed in the public domain, the
 * Work may be freely reproduced, distributed, transmitted, used,
 * modified, built upon, or otherwise exploited by anyone for any
 * purpose, commercial or non-commercial, and in any way, including by
 * methods that have not yet been invented or conceived.
 */

static VALUE_PAIR *example_nr_vp_create(void)
{
	VALUE_PAIR *vp;
	VALUE_PAIR *head = NULL;

	/*
	 *	Create the request contents.
	 */
	vp = nr_vp_create(PW_USER_NAME, 0, "bob", 4);
	if (!vp) {
		fprintf(stderr, "User-Name: %s\n", nr_strerror(0));
		exit(1);
	}
	nr_vps_append(&head, vp);

	/*
	 *	The User-Password attribute is automatically encrypted
	 *	when being placed in the packet.  This version stays
	 *	untouched, and should be "plain text".
	 */
	vp = nr_vp_create(PW_USER_PASSWORD, 0, "hello", 6);
	if (!vp) {
		fprintf(stderr, "User-Password: %s\n", nr_strerror(0));
		exit(1);
	}
	nr_vps_append(&head, vp);

	return head;
}
