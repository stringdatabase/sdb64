/* LNX.C
 * Linux specific functions
 * Copyright (c) 2003 Ladybridge Systems, All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 * START-HISTORY:
 * 31 Dec 23 SD launch - prior history suppressed
 * END-HISTORY
 *
 * START-DESCRIPTION:
 *
 *
 * END-DESCRIPTION
 *
 * START-CODE
 */

#include "sd.h"

/* ======================================================================
   sdsendmail()  -  Send email                                            */

bool sdsendmail(
    sender,
    recipients,
    cc_recipients,
    bcc_recipients,
    subject,
    text,
    attachments) char* sender; /* Sender's address:  fred@acme.com */
char* recipients;              /* Comma separated list of recipient addresses */
char* cc_recipients;           /* Comma separated list of recipient addresses */
char* bcc_recipients;          /* Comma separated list of recipient addresses */
char* subject;                 /* Subject line */
char* text;                    /* Text of email */
char* attachments;             /* Comma separated list of attachment files */
{
  bool status = FALSE;
  /*  20240122  mab change sprintf to snprintf and test for buffer overflow */

  #define tempnamesz 13      /* 12 char + \0 */
  char tempname[tempnamesz]; /* .sd__mailnnnn */
  char command[1024 + 1];
  int tfu;
  int n;

  /* Write mail text to a temporary file */

/*  20240122  mab change sprintf to snprintf and test for buffer overflow */
  if (snprintf(tempname, tempnamesz, ".sd_mail%d", my_uptr->uid) >= tempnamesz) {
    process.status = ER_NO_TEMP;
    process.os_error = errno;
    goto exit_sendmail;
  };
  
  tfu = open(tempname, O_RDWR | O_CREAT | O_TRUNC, default_access);
  if (tfu < 0) {
    process.status = ER_NO_TEMP;
    process.os_error = errno;
    goto exit_sendmail;
  }

  n = strlen(text);
  if (write(tfu, text, n) != n) {
    process.status = ER_NO_TEMP;
    goto exit_sendmail;
  }

  close(tfu);

  /* Construct mail command */

  n = sprintf(command, "mail -s \"%s\"", subject);

  if (cc_recipients != NULL)
    n += sprintf(command + n, " -c %s", cc_recipients);
  if (bcc_recipients != NULL)
    n += sprintf(command + n, " -b %s", bcc_recipients);
  if (recipients != NULL)
    n += sprintf(command + n, " %s", recipients);

  sprintf(command + n, " <%s", tempname);

  system(command);

  /* Delete temporary file */

  remove(tempname);

  status = TRUE;

exit_sendmail:
  return status;
}

/* END-CODE */
