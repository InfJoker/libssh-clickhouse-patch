#define LIBSSH_STATIC

#include "torture.h"
#include "options.c"

ssh_session session;
#ifndef _WIN32
struct passwd pwd;
#endif /* _WIN32 */

static void setup(void) {
    session = ssh_new();
}

static void setup_passwd(void) {
    int rc;
#ifndef _WIN32
#ifndef NSS_BUFLEN_PASSWD
#define NSS_BUFLEN_PASSWD 4096
#endif /* NSS_BUFLEN_PASSWD */
    struct passwd *pwdbuf;
    char buf[NSS_BUFLEN_PASSWD];

    /* get local username */
    rc = getpwuid_r(getuid(), &pwd, buf, NSS_BUFLEN_PASSWD, &pwdbuf);
    ck_assert(rc == 0);
#endif /* _WIN32 */
    session = ssh_new();
}

static void teardown(void) {
    ZERO_STRUCT(pwd);
    ssh_free(session);
}

START_TEST (torture_options_set_host)
{
    int rc;

    rc = ssh_options_set(session, SSH_OPTIONS_HOST, "localhost");
    ck_assert(rc == 0);
    ck_assert_str_eq(session->host, "localhost");

    rc = ssh_options_set(session, SSH_OPTIONS_HOST, "guru@meditation");
    ck_assert(rc == 0);
    ck_assert_str_eq(session->host, "meditation");
    ck_assert_str_eq(session->username, "guru");
}
END_TEST

START_TEST (torture_options_set_port)
{
    int rc;
    unsigned int port = 42;

    rc = ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ck_assert(rc == 0);
    ck_assert(session->port == port);

    rc = ssh_options_set(session, SSH_OPTIONS_PORT_STR, "23");
    ck_assert(rc == 0);
    ck_assert(session->port == 23);

    rc = ssh_options_set(session, SSH_OPTIONS_PORT_STR, "five");
    ck_assert(rc == 0);
    ck_assert(session->port == 0);

    rc = ssh_options_set(session, SSH_OPTIONS_PORT, NULL);
    ck_assert(rc == 0);
    ck_assert(session->port == 22);
}
END_TEST

START_TEST (torture_options_set_fd)
{
    socket_t fd = 42;
    int rc;

    rc = ssh_options_set(session, SSH_OPTIONS_FD, &fd);
    ck_assert(rc == 0);
    ck_assert(session->fd == fd);

    rc = ssh_options_set(session, SSH_OPTIONS_FD, NULL);
    ck_assert(rc == 0);
    ck_assert(session->fd == SSH_INVALID_SOCKET);
}
END_TEST

START_TEST (torture_options_set_user)
{
    int rc;

    rc = ssh_options_set(session, SSH_OPTIONS_USER, "guru");
    ck_assert(rc == 0);
    ck_assert_str_eq(session->username, "guru");


    rc = ssh_options_set(session, SSH_OPTIONS_USER, NULL);
    ck_assert(rc == 0);

#ifndef _WIN32
    ck_assert_str_eq(session->username, pwd.pw_name);
#endif
}
END_TEST

/* TODO */
#if 0
START_TEST (torture_options_set_sshdir)
{
}
END_TEST
#endif

START_TEST (torture_options_set_identity)
{
    int rc;

    rc = ssh_options_set(session, SSH_OPTIONS_ADD_IDENTITY, "identity1");
    ck_assert(rc == 0);
    ck_assert_str_eq(session->identity->root->data, "identity1");

    rc = ssh_options_set(session, SSH_OPTIONS_IDENTITY, "identity2");
    ck_assert(rc == 0);
    ck_assert_str_eq(session->identity->root->data, "identity2");
    ck_assert_str_eq(session->identity->root->next->data, "identity1");
}
END_TEST

Suite *torture_make_suite(void) {
  Suite *s = suite_create("libssh_options");

  torture_create_case_fixture(s, "torture_options_set_host",
          torture_options_set_host, setup, teardown);
  torture_create_case_fixture(s, "torture_options_set_port",
          torture_options_set_port, setup, teardown);
  torture_create_case_fixture(s, "torture_options_set_fd",
          torture_options_set_fd, setup, teardown);
  torture_create_case_fixture(s, "torture_options_set_user",
          torture_options_set_user, setup_passwd, teardown);
  torture_create_case_fixture(s, "torture_options_set_identity",
          torture_options_set_identity, setup, teardown);

  return s;
}

