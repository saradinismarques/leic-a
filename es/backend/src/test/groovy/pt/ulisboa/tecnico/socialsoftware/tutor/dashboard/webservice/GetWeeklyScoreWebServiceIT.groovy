package pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.webservice

import groovyx.net.http.HttpResponseException
import groovyx.net.http.RESTClient
import org.apache.http.HttpStatus
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.domain.Dashboard
import org.springframework.boot.test.context.SpringBootTest
import org.springframework.boot.web.server.LocalServerPort
import pt.ulisboa.tecnico.socialsoftware.tutor.SpockTest
import pt.ulisboa.tecnico.socialsoftware.tutor.user.domain.Teacher
import pt.ulisboa.tecnico.socialsoftware.tutor.user.domain.Student

@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
class GetWeeklyScoreWebServiceIT extends SpockTest {
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
        weeklyScoreService.updateWeeklyScore(dashboardDto.getId())
    }

    def "demo student gets weekly scores"() {
        given: 'a demo student'
        demoStudentLogin()

        when: 'the web service is invoked'
        response = restClient.get(
                path: '/weeklyScores/dashboard/' + dashboardDto.getId(),
                requestContentType: 'application/json'
        )

        then: "the request returns 200"
        response.status == 200
        and: "has value"
        response.data.id != null
        and: 'it is in the database'
        weeklyScoreRepository.findAll().size() == 1

        cleanup:
        weeklyScoreRepository.deleteAll()
    }

    def "demo teacher does not have access"() {
        given: "a teacher"
        demoTeacherLogin()

        when: 'the web service is invoked'
        response = restClient.get(
                path: '/weeklyScores/dashboard/' + dashboardDto.getId(),
                requestContentType: 'application/json'
        )

        then: "the server understands the request and refuses to authorize it"
        def error = thrown(HttpResponseException)
        error.response.status == HttpStatus.SC_FORBIDDEN

        cleanup:
        weeklyScoreRepository.deleteAll()
    }

    def "new demo student does not have access"() {
        given: 'a different student'
        def loginResponse = restClient.get(
                path: '/auth/demo/student',
                query: ['createNew': true]
        )
        restClient.headers['Authorization']  = "Bearer " + loginResponse.data.token

        when: 'the web service is invoked'
        response = restClient.get(
                path: '/weeklyScores/dashboard/' + dashboardDto.getId(),
                requestContentType: 'application/json'
        )

        then: "the server understands the request and refuses to authorize it"
        def error = thrown(HttpResponseException)
        error.response.status == HttpStatus.SC_FORBIDDEN

        cleanup:
        weeklyScoreRepository.deleteAll()
    }

}