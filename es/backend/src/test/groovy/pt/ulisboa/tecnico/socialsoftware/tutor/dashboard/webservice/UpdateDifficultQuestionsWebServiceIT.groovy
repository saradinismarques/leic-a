package pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.webservice

import groovyx.net.http.HttpResponseException
import groovyx.net.http.RESTClient
import org.apache.http.HttpStatus
import org.springframework.boot.test.context.SpringBootTest
import org.springframework.boot.web.server.LocalServerPort
import pt.ulisboa.tecnico.socialsoftware.tutor.SpockTest
import pt.ulisboa.tecnico.socialsoftware.tutor.answer.domain.QuestionAnswer
import pt.ulisboa.tecnico.socialsoftware.tutor.answer.domain.QuizAnswer
import pt.ulisboa.tecnico.socialsoftware.tutor.auth.domain.AuthUser
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.domain.Dashboard
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.service.FailedAnswersSpockTest
import pt.ulisboa.tecnico.socialsoftware.tutor.question.domain.MultipleChoiceQuestion
import pt.ulisboa.tecnico.socialsoftware.tutor.question.domain.Option
import pt.ulisboa.tecnico.socialsoftware.tutor.question.domain.Question
import pt.ulisboa.tecnico.socialsoftware.tutor.quiz.domain.Quiz
import pt.ulisboa.tecnico.socialsoftware.tutor.quiz.domain.QuizQuestion
import pt.ulisboa.tecnico.socialsoftware.tutor.user.domain.Student
import pt.ulisboa.tecnico.socialsoftware.tutor.user.domain.Teacher
import pt.ulisboa.tecnico.socialsoftware.tutor.utils.DateHandler

@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
class UpdateDifficultQuestionsWebServiceIT extends SpockTest {
    @LocalServerPort
    private int port

    def response
    def student
    def dashboard

    def setup() {
        given:
        restClient = new RESTClient("http://localhost:" + port)
        and:
        createExternalCourseAndExecution()
        and:
        def now = DateHandler.now()
        and:
        student = new Student(USER_1_NAME, USER_1_EMAIL, USER_1_EMAIL, false, AuthUser.Type.EXTERNAL)
        student.authUser.setPassword(passwordEncoder.encode(USER_1_PASSWORD))
        student.addCourse(externalCourseExecution)
        userRepository.save(student)
        and:
        def question = new Question()
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
        def optionOK = new Option()
        optionOK.setContent(OPTION_1_CONTENT)
        optionOK.setCorrect(true)
        optionOK.setSequence(0)
        optionOK.setQuestionDetails(questionDetails)
        optionRepository.save(optionOK)
        and:
        def optionKO = new Option()
        optionKO.setContent(OPTION_1_CONTENT)
        optionKO.setCorrect(false)
        optionKO.setSequence(1)
        optionKO.setQuestionDetails(questionDetails)
        optionRepository.save(optionKO)
        and:
        def quiz = new Quiz()
        quiz.setAvailableDate(now.minusMinutes(5))
        quiz.setResultsDate(now)
        quiz.setCourseExecution(externalCourseExecution)
        quizRepository.save(quiz)
        and:
        def quizQuestion = new QuizQuestion()
        quizQuestion.setQuiz(quiz)
        quizQuestion.setQuestion(question)
        quizQuestionRepository.save(quizQuestion)
        and:
        def quizAnswer = new QuizAnswer()
        quizAnswer.setAnswerDate(now.minusMinutes(1))
        quizAnswer.setQuiz(quiz)
        quizAnswer.setStudent(student)
        quizAnswerRepository.save(quizAnswer)
        and:
        def questionAnswer = new QuestionAnswer()
        questionAnswer.setQuizQuestion(quizQuestion)
        questionAnswer.setQuizAnswer(quizAnswer)
        questionAnswerRepository.save(questionAnswer)
        and:
        dashboard = new Dashboard(externalCourseExecution, student)
        dashboardRepository.save(dashboard)
    }

    def "student updates difficult questions"() {
        given: 'a student'
        createdUserLogin(USER_1_EMAIL, USER_1_PASSWORD)
        
        when: 'the web service is invoked'
        response = restClient.put(
            path: '/difficultQuestions/dashboard/' + dashboard.getId(),
            requestContentType: 'application/json'
        )

        then: "the request returns 200"
        response != null
        response.status == 200
        and: "the difficult question is in the repository"
        difficultQuestionRepository.findAll().size() == 1
        and: "the difficult question is in the dashboard"
        difficultQuestionRepository.findAll().get(0).getDashboard().getId() == dashboard.getId()

        cleanup:
        difficultQuestionRepository.deleteAll()
    }

    def "teacher cant update student's difficult questions"() {
        given: 'a teacher'
        def teacher = new Teacher(USER_2_NAME, USER_2_EMAIL, USER_2_EMAIL, false, AuthUser.Type.TECNICO)
        teacher.authUser.setPassword(passwordEncoder.encode(USER_2_PASSWORD))
        userRepository.save(teacher)
        createdUserLogin(USER_2_EMAIL, USER_2_PASSWORD)

        when: 'the web service is invoked'
        response = restClient.put(
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

    def "student cant update another students difficult questionss"() {
        given: 'a different student'
        def student2 = new Student(USER_3_NAME, USER_3_EMAIL, USER_3_EMAIL, false, AuthUser.Type.EXTERNAL)
        student2.authUser.setPassword(passwordEncoder.encode(USER_2_PASSWORD))
        student2.addCourse(externalCourseExecution)
        userRepository.save(student2)
        createdUserLogin(USER_3_EMAIL, USER_2_PASSWORD)

        when: 'the web service is invoked'
        response = restClient.put(
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