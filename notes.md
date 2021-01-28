For quest+, the psychometric function knows at compile time the stimulus domain (i.e. the continuous independent vars), the parameter domain (e.g. threshold, slope, bounds), the outcome domain (which is always 1D, and is generally {0, 1} (more for multinomial))

So inheritance-wise, we have the super-generic Base for `update` and `next`

```c++
template <std::size_t DimStim = 1>
class Base
{

protected:
    typedef std::conditional_t<(DimStim > 1), std::array<double, DimStim>, double> stim_type;

public:
    virtual stim_type next() = 0;
    virtual bool update(int response,
                        const std::optional<stim_type> intensity = std::nullopt) = 0;
};
```

The base quest+ psychometric function, which also sets the right type for `next` and `update` based on the `DimStim`. We _should_ be able to specify `next` and `update` now too?

```c++

template <std::size_t DimStim, std::size_t DimParam, std::size_t NResp = 2>
class QuestPlusBase : public Base<DimStim>
{
    public:

        QuestPlusBase(const QuestPlusParams &qp_params) : qp_params(qp_params) {
            // fill in basic init
        }

        using stim_type = typename Base<DimStim>::stim_type;
        // TODO: fill in next & update
        stim_type next() {

        }
        bool update(int response, std::optional<stim_type> intensity) {

        }

    private:
        xt::xtensor<double, DimParam> prior;
        xt::xtensor<double, DimParam> posterior;
        xt::xtensor<double, DimParam+DimStim+1> likelihoods;
        std::vector<int> responses;      // history of user responses
        std::vector<double> intensities; // history of intensities

    protected:
        const QuestPlusParams qp_params;
        // derived classes must tell us how to calc prior & likelihood
        virtual xt::xtensor<double, DimParam> generate_prior() = 0;
        // +1 for response dimension
        virtual xt::xtensor<double, DimParam+DimStim+1> generate_likelihoods() = 0;
        static constexpr std::size_t dim_stim = DimStim;
        static constexpr std::size_t dim_param = DimParam;
        static constexpr std::size_t n_resp = NResp;

        Base<DimStim>::stim_type next_intensities; // store previous vals


        void setup() {
            // everything else for init, post-assigning settings
            prior = generate_prior();
            posterior = prior;
            likelihoods = generate_likelihoods();
            //
        }

        xt::xtensor<double, DimParam> prior_helper(const std::vector<double> &param,
                                                   const std::optional<std::vector<double>>& prior = std::nullopt,
                                                   const std::size_t index = 0)
        {
            const auto param_size = param.size();
            std::array<std::size_t, DimParam> prior_shape;
            prior_shape.fill(1);
            prior_shape[index] = param_size;
            xt::xtensor<double, DimParam> out_prior;
            if (prior) {
                auto &tp = *prior;
                if (tp.size() != param_size) {
                    throw std::invalid_argument("The prior and parameter domain sizes must match.");
                }
                out_prior = xt::adapt(tp, prior_shape);
            } else {
                out_prior = xt::ones<double>(prior_shape);
            }
            return out_prior;
        }
};
```

(does order of dims matter for performance?)
(be aware of order of parameters in prior, and params/stim/resp order in likelihood)

Derived psychometric functions (e.g. normal CDF)

```c++
class Weibull : public QuestPlusBase<1, 4, 2>
{
    public:
        Weibull(const QuestPlusParams &qp_params, const WeibullParams &weibull_settings) : QuestPlusBase<1, 4, 2>(qp_params), settings(weibull_settings)
        {

        }
    protected:
        const WeibullParams settings;

        xt::xtensor<double, Weibull::dim_param> generate_prior() {
            auto thresh_prior = prior_helper(settings.threshold, settings.threshold_prior, 0);
            auto slope_prior = prior_helper(settings.slope, settings.slope_prior, 1);
            auto lower_prior = prior_helper(settings.lower_asymptote,
                                            settings.lower_asymptote_prior, 2);
            auto lapse_prior = prior_helper(settings.lapse_rate, settings.lapse_rate_prior, 3);
            auto prior = thresh_prior * slope_prior * lower_prior * lapse_prior;
            return prior / xt::sum(prior);
        }

        xt::xtensor<double, Weibull::dim_param+Weibull::dim_stim+1> generate_likelihoods() {
            using sz = std::array<std::size_t, Weibull::dim_param+Weibull::dim_stim+1>;
            auto x = xt::adapt(settings.intensity, sz{settings.intensity.size(), 1, 1, 1, 1});
            auto thresh = xt::adapt(settings.threshold, sz{1, settings.threshold.size(), 1, 1, 1});
            auto slope = xt::adapt(settings.slope, sz{1, 1, settings.slope.size(), 1, 1});
            auto lower = xt::adapt(settings.lower_asymptote, sz{1, 1, 1, settings.lower_asymptote.size(), 1});
            auto lapse = xt::adapt(settings.lapse_rate, sz{1, 1, 1, 1, settings.lapse_rate.size()});

            xt::xtensor<double, Weibull::dim_param+Weibull::dim_stim> p;
            switch (settings.scale)
            {
            case Scale::Linear:
                p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(x / thresh, slope));
                break;
            case Scale::Log10:
                p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(10, slope * (x - thresh)));
                break;
            case Scale::dB:
                p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(10, slope * (x - thresh) / 20.0));
                break;
            }
            // in this, we diverge from hoechenberger/questplus
            // store 0/incorrect as 0th element, so that we can index using the response
            return xt::stack(xt::xtuple(1.0 - p, p));
        }

};
```
