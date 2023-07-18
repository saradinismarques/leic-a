package pt.ulisboa.tecnico.socialsoftware.tutor.dashboard;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Isolation;
import org.springframework.transaction.annotation.Transactional;

import pt.ulisboa.tecnico.socialsoftware.tutor.answer.domain.QuestionAnswer;
import pt.ulisboa.tecnico.socialsoftware.tutor.answer.repository.QuestionAnswerRepository;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.domain.Dashboard;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.domain.DifficultQuestion;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.dto.DifficultQuestionDto;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.repository.DashboardRepository;
import pt.ulisboa.tecnico.socialsoftware.tutor.dashboard.repository.DifficultQuestionRepository;
import pt.ulisboa.tecnico.socialsoftware.tutor.exceptions.ErrorMessage;
import pt.ulisboa.tecnico.socialsoftware.tutor.exceptions.TutorException;
import pt.ulisboa.tecnico.socialsoftware.tutor.question.domain.Question;
import pt.ulisboa.tecnico.socialsoftware.tutor.question.repository.QuestionRepository;
import pt.ulisboa.tecnico.socialsoftware.tutor.quiz.domain.QuizQuestion;
import pt.ulisboa.tecnico.socialsoftware.tutor.utils.DateHandler;

import static pt.ulisboa.tecnico.socialsoftware.tutor.exceptions.ErrorMessage.*;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

@Service
public class DifficultQuestionService {

    @Autowired
    private QuestionAnswerRepository questionAnswerRepository;

    @Autowired
    private QuestionRepository questionRepository;

    @Autowired
    private DashboardRepository dashboardRepository;

    @Autowired
    private DifficultQuestionRepository difficultQuestionRepository;

    @Transactional(isolation = Isolation.READ_COMMITTED)
    public DifficultQuestionDto createDifficultQuestion(int dashboardId, int questionId, int percentage) {
        Dashboard dashboard = dashboardRepository.findById(dashboardId).orElseThrow(() -> new TutorException(ErrorMessage.DASHBOARD_NOT_FOUND, dashboardId));
        Question question = questionRepository.findById(questionId).orElseThrow(() -> new TutorException(QUESTION_NOT_FOUND, questionId));

        DifficultQuestion difficultQuestion = new DifficultQuestion(dashboard, question, percentage);
        difficultQuestionRepository.save(difficultQuestion);

        return new DifficultQuestionDto(difficultQuestion);
    }

    @Transactional(isolation = Isolation.READ_COMMITTED)
    public void removeDifficultQuestion(int difficultQuestionId) {
        DifficultQuestion difficultQuestion = difficultQuestionRepository.findById(difficultQuestionId).orElseThrow(() -> new TutorException(DIFFICULT_QUESTION_NOT_FOUND, difficultQuestionId));

        difficultQuestion.setRemoved(true);
        difficultQuestion.setRemovedDate(DateHandler.now());
    }

    @Transactional(isolation = Isolation.READ_COMMITTED)
    public List<DifficultQuestionDto> getDifficultQuestions(int dashboardId) {
        Dashboard dashboard = dashboardRepository.findById(dashboardId).orElseThrow(() -> new TutorException(ErrorMessage.DASHBOARD_NOT_FOUND, dashboardId));

        return dashboard.getDifficultQuestions().stream()
            .filter(difficultQuestion -> difficultQuestion.isRemoved() == false)
            .map(difficultQuestion -> new DifficultQuestionDto(difficultQuestion))
            .collect(Collectors.toList());
    } 

    @Transactional(isolation = Isolation.READ_COMMITTED)
    public void updateDifficultQuestions(int dashboardId) {
        Dashboard dashboard = dashboardRepository.findById(dashboardId).orElseThrow(() -> new TutorException(ErrorMessage.DASHBOARD_NOT_FOUND, dashboardId));

        LocalDateTime now = DateHandler.now();

        Set<DifficultQuestion> questionsToRemove = dashboard.getDifficultQuestions().stream()
                .filter(difficultQuestion -> !difficultQuestion.isRemoved() ||
                        (difficultQuestion.isRemoved() && difficultQuestion.getRemovedDate().plusDays(7).isBefore((now))))
                .collect(Collectors.toSet());

        questionsToRemove.forEach(difficultQuestion -> {
            difficultQuestion.remove();
            difficultQuestionRepository.delete(difficultQuestion);
        });

        Set<Question> questionsToPersist = dashboard.getDifficultQuestions().stream()
                .map(DifficultQuestion::getQuestion)
                .collect(Collectors.toSet());

        dashboard.getCourseExecution().getQuizzes().stream()
                .filter(quiz -> quiz.getResultsDate() == null || quiz.getResultsDate().isAfter(now.minusDays(7)))
                .flatMap(quiz -> quiz.getQuizQuestions().stream()
                        .map(QuizQuestion::getQuestion))
                .distinct()
                .filter(question -> !questionsToPersist.contains(question))
                .forEach(question -> {
                    int percentageCorrect = percentageCorrect(question, now.minusDays(7));
                    if (percentageCorrect < 25) {
                        DifficultQuestion difficultQuestion = new DifficultQuestion(dashboard, question, percentageCorrect);
                        difficultQuestionRepository.save(difficultQuestion);
                    }
                });

        dashboard.setLastCheckDifficultQuestions(now);
    }

    private int percentageCorrect(Question question, LocalDateTime weekAgo) {
        Set<QuestionAnswer> answersInWeek = question.getQuizQuestions().stream()
                .flatMap(quizQuestion -> quizQuestion.getQuestionAnswers().stream())
                .filter(questionAnswer -> questionAnswer.getQuizAnswer().getAnswerDate().isAfter(weekAgo))
                .collect(Collectors.toSet());

        int totalAnswers = answersInWeek.size();

        if (totalAnswers == 0) {
            return 100;
        }

        int correctAnswers = (int) answersInWeek.stream()
                .filter(QuestionAnswer::isCorrect)
                .count();

        return (correctAnswers * 100) / totalAnswers;
    }
}