package pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.webservice

import groovyx.net.http.HttpResponseException
import groovyx.net.http.RESTClient
import org.apache.http.HttpStatus
import org.springframework.boot.test.context.SpringBootTest
import org.springframework.boot.web.server.LocalServerPort
import pt.ulisboa.tecnico.socialsoftware.tutor.SpockTest
import pt.ulisboa.tecnico.socialsoftware.tutor.auth.domain.AuthUser
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.domain.Dashboard
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.domain.DifficultQuestion
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.service.FailedAnswersSpockTest
import pt.ulisboa.tecnico.socialsoftware.tutor.execution.repository.CourseExecutionRepository
import pt.ulisboa.tecnico.socialsoftware.tutor.question.domain.MultipleChoiceQuestion
import pt.ulisboa.tecnico.socialsoftware.tutor.question.domain.Option
import pt.ulisboa.tecnico.socialsoftware.tutor.question.domain.Question
import pt.ulisboa.tecnico.socialsoftware.tutor.user.domain.Student
import pt.ulisboa.tecnico.socialsoftware.tutor.user.domain.Teacher

import java.time.LocalDateTime

@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
class GetDifficultQuestionWebServiceIT extends SpockTest {
    @LocalServerPort
    private int port

    def response
    def student
    def dashboard
    def question
    def optionOK
    def optionKO

    def setup() {
        given:
        restClient = new RESTClient("http://localhost:" + port)
        and:
        createExternalCourseAndExecution()
        and:
        student = new Student(USER_1_NAME, USER_1_EMAIL, USER_1_EMAIL, false, AuthUser.Type.EXTERNAL)
        student.authUser.setPassword(passwordEncoder.encode(USER_1_PASSWORD))
        student.addCourse(externalCourseExecution)
        userRepository.save(student)
        and:
        question = new Question()
        question.setKey(1)
        question.setTitle(QUESTION_1_TITLE)
        question.setContent(QUESTION_1_CONTENT)
        question.setStatus(Question.Status.AVAILABLE)
        question.setNumberOfAnswers(2)
        question.setNumberOfCorrect(1)
        question.setCourse(externalCourse)
        def questionDetails = new MultipleChoiceQuestion()
        question.setQuestionDetails(questionDetails)
        questionDetailsRepository.save(questionDetails)
        questionRepository.save(question)
        and:
        optionOK = new Option()
        optionOK.setContent(OPTION_1_CONTENT)
        optionOK.setCorrect(true)
        optionOK.setSequence(0)
        optionOK.setQuestionDetails(questionDetails)
        optionRepository.save(optionOK)
        and:
        optionKO = new Option()
        optionKO.setContent(OPTION_1_CONTENT)
        optionKO.setCorrect(false)
        optionKO.setSequence(1)
        optionKO.setQuestionDetails(questionDetails)
        optionRepository.save(optionKO)
        and:
        dashboard = new Dashboard(externalCourseExecution, student)
        dashboardRepository.save(dashboard)
    }

    def "student gets difficult questions"() {
        given: 'a student'
        createdUserLogin(USER_1_EMAIL, USER_1_PASSWORD)
        and: 'its difficult questions'
        def difficultQuestionDto = difficultQuestionService.createDifficultQuestion(dashboard.getId(), question.getId(), 24)

        when: 'the web service is invoked'
        response = restClient.get(
            path: '/difficultQuestions/dashboard/' + dashboard.getId(),
            requestContentType: 'application/json'
        )

        then: "the request returns 200"
        response != null
        response.status == 200
        and: "it is the same difficult question"
        response.data[0].id == difficultQuestionDto.id
        and: 'it is in the database'
        difficultQuestionRepository.findAll().size() == 1

        cleanup:
        difficultQuestionRepository.deleteAll()
    }

    def "teacher can't get student's difficult questions"() {
        given: 'a teacher'
        def teacher = new Teacher(USER_2_NAME, USER_2_EMAIL, USER_2_EMAIL, false, AuthUser.Type.TECNICO)
        teacher.authUser.setPassword(passwordEncoder.encode(USER_2_PASSWORD))
        userRepository.save(teacher)
        createdUserLogin(USER_2_EMAIL, USER_2_PASSWORD)
        
        when: 'the web service is invoked'
        response = restClient.get(
                path: '/difficultQuestions/dashboard/' + dashboard.getId(),
                requestContentType: 'application/json'
        )

        then: "the server understands the request and refuses to authorize it"
        response == null
        def error = thrown(HttpResponseException)
        error.response.status == HttpStatus.SC_FORBIDDEN

        cleanup:
        userRepository.deleteById(teacher.getId())
    }

    def "student can't get another student's difficult questions"() {
        given: 'a different student'
        def student2 = new Student(USER_3_NAME, USER_3_EMAIL, USER_3_EMAIL, false, AuthUser.Type.EXTERNAL)
        student2.authUser.setPassword(passwordEncoder.encode(USER_2_PASSWORD))
        student2.addCourse(externalCourseExecution)
        userRepository.save(student2)
        createdUserLogin(USER_3_EMAIL, USER_2_PASSWORD)

        when: 'the web service is invoked'
        response = restClient.get(
                path: '/difficultQuestions/dashboard/' + dashboard.getId(),
                requestContentType: 'application/json'
        )

        then: "the server understands the request and refuses to authorize it"
        response == null
        def error = thrown(HttpResponseException)
        error.response.status == HttpStatus.SC_FORBIDDEN

        cleanup:
        userRepository.deleteById(student2.getId())
    }

    def cleanup() {
        dashboardRepository.deleteAll()
        userRepository.deleteById(student.getId())
        courseRepository.deleteById(externalCourse.getId())
    }
}