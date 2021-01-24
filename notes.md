For quest+, the psychometric function knows at compile time the stimulus domain (i.e. the continuous independent vars), the parameter domain (e.g. threshold, slope, bounds), the outcome domain (which is always 1D, and is generally {0, 1} (more for multinomial))

So inheritance-wise, we have the super-generic Base for `update` and `next`

```c++
template <typename T = double>
class Base
{
    public:
        virtual T next() = 0;
        virtual bool update(int response, std::optional<T> intensity) = 0;
    protected:
        typedef T stim_type;
};
```

The base quest+ psychometric function, which also sets the right type for `next` and `update` based on the `DimStim`. We _should_ be able to specify `next` and `update` now too?

```c++

template <std::size_t DimParam, std::size_t DimStim, std::size_t NResp = 2>
class PsychometricFunction : public Base<std::conditional_t<(DimStim > 1),
                                                            std::array<double, DimStim>,
                                                            double>>
{
    public:
        using stim_type = typename PsychometricFunction<DimParam, DimStim, NResp>::stim_type;
        // TODO: fill in next & update
    protected:
        virtual xt::xtensor<double, DimParam> generate_prior() = 0;
        // +1 for response dimension
        virtual xt::xtensor<double, DimParam+DimStim+1> generate_likelihoods() = 0;
        static constexpr std::size_t dim_param = DimParam;
        static constexpr std::size_t dim_stim = DimStim;
        static constexpr std::size_t n_resp = NResp;
};
```

(does order of dims matter for performance?)
(be aware of order of parameters in prior, and params/stim/resp order in likelihood)

Derived psychometric functions (e.g. normal CDF)

```c++
class NormCDF : public PsychometricFunction<4, 1, 2>
{

};
```
