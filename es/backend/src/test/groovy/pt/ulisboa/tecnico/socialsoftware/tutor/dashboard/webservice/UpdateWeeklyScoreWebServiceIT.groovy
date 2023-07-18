package pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.webservice

import groovyx.net.http.HttpResponseException
import groovyx.net.http.RESTClient
import org.apache.http.HttpStatus
import org.springframework.boot.test.context.SpringBootTest
import org.springframework.boot.web.server.LocalServerPort
import pt.ulisboa.tecnico.socialsoftware.tutor.SpockTest
import pt.ulisboa.tecnico.socialsoftware.tutor.user.domain.Student
import pt.ulisboa.tecnico.socialsoftware.tutor.auth.domain.AuthUser

@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
class UpdateWeeklyScoreWebServiceIT extends SpockTest {
    @LocalServerPort
    private int port

    def response

    def authUserDto
    def courseExecutionDto
    def dashboardDto

    def setup() {
        given:
        restClient = new RESTClient("http://localhost:" + port)
        and:
        courseExecutionDto = courseService.getDemoCourse()
        authUserDto = authUserService.demoStudentAuth(false).getUser()
        dashboardDto = dashboardService.getDashboard(courseExecutionDto.getCourseExecutionId(), authUserDto.getId())
    }

    def "demo student gets its weekly scores"() {
        given:'create weekly score and login with student'
        demoStudentLogin()

        when: 'the web service is invoked'
        response = restClient.put(
                path: '/students/updateweeklyscore/dashboard/' + dashboardDto.getId(),
                requestContentType: 'application/json'
        )

        then: "the request returns 200"
        response.status == 200
        and: 'it is in the database'
        weeklyScoreRepository.findAll().size() == 1

        cleanup:
        weeklyScoreRepository.deleteAll()
    }

    def "demo teacher does not have access"() {
        given: 'a teacher'
        demoTeacherLogin();

        when: 'the web service is invoked'
        response = restClient.put(
                path: '/students/updateweeklyscore/dashboard/' + dashboardDto.getId(),
                requestContentType: 'application/json'
        )

        then: "the server understands the request but refuses to authorize it"
        def error = thrown(HttpResponseException)
        error.response.status == HttpStatus.SC_FORBIDDEN
        and:
        weeklyScoreRepository.findAll().size() == 0
    }

    def "student cant update another students weekly scores"() {
        given: 'a student'
        def student2 = new Student(USER_2_NAME, USER_2_USERNAME, USER_2_EMAIL, false, AuthUser.Type.TECNICO)
        student2.authUser.setPassword(passwordEncoder.encode(USER_2_PASSWORD))
        userRepository.save(student2)
        createdUserLogin(USER_2_USERNAME,USER_2_PASSWORD)

        when: 'the web service is invoked'
        response = restClient.put(
                path: '/students/updateweeklyscore/dashboard/' + dashboardDto.getId(),
                requestContentType: 'application/json'
        )

        then: "the server understands the request but refuses to authorize it"
        def error = thrown(HttpResponseException)
        error.response.status == HttpStatus.SC_FORBIDDEN
        and:
        weeklyScoreRepository.findAll().size() == 0

        cleanup:
        userRepository.deleteById(student2.getId())
    }

}